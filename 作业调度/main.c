#include <stdio.h>
#include <stdlib.h>
#define MAX 999

typedef struct tag_pcb
{
    char name[8];
    int arrive;           //到达时间
    int need;             //运行时间
    int end;              //完成时间
    int ok;               //0表示已运行，1表示未运行
    double response;      //响应比
    int turn_time;        //周转时间
    double avg_turn_time; //带权周转时间
    struct tag_pcb *next;
} PCB;

void CreateRQ(PCB *RQ, char *filename)
{ //从文件读取数据创建进程链表
    FILE *fp;
    if ((fp = fopen(filename, "r")) == NULL)
    {
        printf("Can't open file %s\n", filename);
        exit(0);
    }

    PCB *p = RQ;
    while (!feof(fp))
    {
        PCB *q = (PCB *)malloc(sizeof(PCB));
        fscanf(fp, "%s %d %d", q->name, &q->arrive, &q->need);
        q->ok = 1; //未运行
        p->next = q;
        p = q;
    }
    p->next = NULL;
    fclose(fp);
}

void display(PCB *RQ)
{ //打印队列
    PCB *p;
    p = RQ->next;
    for (; p != NULL; p = p->next)
        printf("%s\t%lf\t%d\n", p->name, p->response, p->ok);
}

int check(PCB *RQ)
{ //检查队列中进程是否全部运行结束
    PCB *p;
    p = RQ->next;
    while (p != NULL && p->ok == 0) //跳过已运行完成的进程
        p = p->next;

    if (p == NULL)
        return 0;
    else
        return 1;
}

void cpu_process(int *clockA, int *clockB, PCB *pwork)
{ //进程运行
    int *clock;
    if (*clockA <= *clockB)
        clock = clockA;
    else
        clock = clockB;
    if ((*clock) < pwork->arrive)
        (*clock) = pwork->arrive;
    (*clock) += pwork->need;
    pwork->end = (*clock); //记录结束时间
    pwork->turn_time = pwork->end - pwork->arrive;
    pwork->avg_turn_time = 1.0 * pwork->turn_time / pwork->need;
    pwork->ok = 0; //已运行
    printf("%s ", pwork->name);
}

void time(PCB *RQ)
{ //打印平均周转时间、平均带权周转时间
    PCB *p = RQ->next;
    double time = 0.0, weight_time = 0.0;
    int count = 0;
    while (p != NULL)
    {
        time += p->turn_time;
        weight_time += p->avg_turn_time;
        count++;
        p = p->next;
    }

    printf("Average turnaround time = %.3lf\n", time / count);
    printf("average turnaround time with the right = %.3lf\n", weight_time / count);
}

void FCFS(char *filename)
{ //先来先服务
    printf("FCFS:\n");
    PCB *RQ = (PCB *)malloc(sizeof(PCB)); //初始化头节点
    CreateRQ(RQ, filename);               //创建队列
    //display(RQ);
    int clockA = 0;
    int clockB = 0;
    PCB *p = RQ->next;
    PCB *pfirst = RQ->next;
    int count = 1;
    while (count && p != NULL)
    {
        while (p != NULL)
        { //选出先到达进程
            if (p->ok == 1 || pfirst->ok == 0)
            {
                if (pfirst->ok == 0)
                    pfirst = pfirst->next;
                else if (pfirst->arrive > p->arrive)
                    pfirst = p;
            }
            p = p->next;
        }
        cpu_process(&clockA, &clockB, pfirst);
        p = RQ->next;
        pfirst = RQ->next;

        count = check(RQ);
    }
    printf("\n");
    time(RQ);
    free(RQ);
    printf("\n");
}

void SJF(char *filename)
{ //短进程优先
    printf("SJF: \n");
    PCB *RQ = (PCB *)malloc(sizeof(PCB)); //初始化头节点
    CreateRQ(RQ, filename);               //创建队列
    int clockA = 0;
    int clockB = 0;
    PCB *p = RQ->next;
    PCB *pmin = RQ->next;
    int count = 1;
    int queue_count; //记录就绪队列中是否有进程
    while (count && p != NULL)
    {
        queue_count = 0;
        while (p != NULL)
        { //选出待运行的短进程
            if (p->ok == 1 || pmin->ok == 0)
            {
                if (pmin->ok == 0)
                {
                    pmin = pmin->next;
                    if (queue_count == 0)
                        queue_count++;
                }
                else if (p->arrive > clockA || p->arrive > clockB)
                {
                    if (queue_count > 0)
                    {
                        if (pmin->arrive > p->arrive && pmin->need > p->need)
                        {
                            pmin = p;
                            queue_count++;
                        }
                    }
                    else if (pmin->arrive > p->arrive)
                    {
                        pmin = p;
                        queue_count++;
                    }
                }
                else if (p->arrive <= clockA && p->arrive <= clockB)
                {
                    if (pmin->need > p->need)
                    {
                        pmin = p;
                        queue_count++;
                    }
                }
            }
            p = p->next;
        }
        cpu_process(&clockA, &clockB, pmin);
 
        p = RQ->next;
        pmin = RQ->next;

        count = check(RQ);
    }
    printf("\n");
    time(RQ);
    free(RQ);
    printf("\n");
}

void HRRN(char *filename)
{ //高响应比优先
    printf("HRRN: \n");
    PCB *RQ;
    RQ = (PCB *)malloc(sizeof(PCB)); //初始化头节点
    CreateRQ(RQ, filename);          //创建队列
    int clockA = 0;
    int clockB = 0;
    PCB *p = RQ->next;
    PCB *pmax = RQ->next;
    int count = 1;
    int queue_count;
    while (count && p != NULL)
    {
        queue_count = 0;
        while (p != NULL)
        {
            if (p->ok == 0)
                ;
            else
            {
                int clock;
                clock = clockA < clockB ? clockA : clockB;
                if (p->arrive > clock)
                    p->response = MAX;
                else
                    p->response = 1.0 * (clock - p->arrive) / p->need + 1;
            }
            p = p->next;
        }
        //display(RQ);
        p = RQ->next;
        while (p != NULL)
        { //选出高响应比进程
            if (p->ok == 1 || pmax->ok == 0)
            {
                if (pmax->ok == 0)
                {
                    pmax = pmax->next;
                    if (queue_count == 0)
                        queue_count++;
                }
                else if (p->arrive > clockA || p->arrive > clockB)
                {
                    if (queue_count > 0)
                    {
                        if (pmax->arrive > p->arrive && pmax->response < p->response)
                        {
                            pmax = p;
                            queue_count++;
                        }
                    }
                    else if (pmax->arrive > p->arrive)
                    {
                        pmax = p;
                        queue_count++;
                    }
                }
                else if (p->arrive <= clockA && p->arrive <= clockB)
                {
                    if (pmax->response < p->response)
                    {
                        pmax = p;
                        queue_count++;
                    }
                }
            }
            p = p->next;
        }

        cpu_process(&clockA, &clockB, pmax);
        p = RQ->next;
        pmax = RQ->next;

        count = check(RQ);
    }
    printf("\n");
    time(RQ);
    free(RQ);
    printf("\n");
}

int main()
{
    char filename[10] = "RQ.txt";
    FCFS(filename);
    SJF(filename);
    HRRN(filename);
    return 0;
}
