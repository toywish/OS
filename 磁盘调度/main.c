#include <stdio.h>
#include <stdlib.h>
#define init_track 90 //当前磁道号
#define MAX_track 200 //最大磁道号
typedef struct tag_track
{
    char name[3];
    int id;     //访问的磁道号
    int dis;    //移动的磁道数
    int status; //记录状态
    struct tag_track *next;
} track;

void init(track *list, char *filename)
{ //从文件读取数据创建链表
    FILE *fp;
    if ((fp = fopen(filename, "r")) == NULL)
    {
        printf("Can't open file %s\n", filename);
        exit(0);
    }
    track *p = list;
    while (!feof(fp))
    {
        track *q = (track *)malloc(sizeof(track));
        fscanf(fp, "%s %d", q->name, &q->id);
        q->status = 1;
        p->next = q;
        p = q;
    }
    p->next = NULL;
    fclose(fp);
}

void display(track *list)
{ //打印测试数据
    track *p;
    p = list->next;
    while (p != NULL)
    {
        printf("%s %d\n", p->name, p->id);
        p = p->next;
    }
}

int check(track *list)
{ //检查队列中所有服务是否全部响应结束
    track *p;
    p = list->next;
    while (p != NULL && p->status == 0)
        p = p->next;
    if (p == NULL)
        return 0;
    else
        return 1;
}

void check_direction(track *list, track *pwork, int *direction)
{ //判断磁道是否需要转变方向
    track *p = list->next;
    track *pm = list->next;
    int cnt = (*direction);
    if (cnt == 1)
        while (p != NULL)
        {
            if (pm->id < p->id)
                pm = p;
            p = p->next;
        }
    else if (cnt == 0)
        while (p != NULL)
        {
            if (pm->id < p->id)
                pm = p;
            p = p->next;
        }
    if (pm==pwork)
        cnt = cnt==1?0:1;
    (*direction) = cnt;
}

void avg_move_track(track *list)
{ //计算平均移动磁道数
    double sum = 0.0;
    int count = 0;
    track *p;
    p = list->next;
    while (p != NULL)
    {
        sum += p->dis;
        count++;
        p = p->next;
    }
    printf("Average number of tracks moved: %lf\n", sum / count);
}

void track_process(int *current_track, track *pwork)
{ //访问相应磁道
    pwork->dis = abs(*current_track - pwork->id);
    *current_track = pwork->id;
    pwork->status = 0; //表示此服务已响应完成
    printf(" %s", pwork->name);
}

void fcfs(char *filename)
{ //先来先服务算法
    printf("fcfs:");
    track *list;
    list = (track *)malloc(sizeof(track));
    init(list, filename);
    track *pwork = list->next;
    int current_track = init_track;
    while (pwork != NULL)
    {
        track_process(&current_track, pwork);
        pwork = pwork->next;
    }
    printf("\n");
    avg_move_track(list);
    free(list);
}

void sstf(char *filename)
{ //最短寻道优先
    printf("sstf:");
    track *list;
    list = (track *)malloc(sizeof(track));
    init(list, filename);
    track *p = list->next;
    track *pwork = list->next;
    int current_track = init_track;
    int cnt = 1;
    while (cnt && p != NULL)
    {
        while (p != NULL)
        { //寻找最近磁道
            if (p->status != 0)
                if (pwork->status == 0)
                {
                    pwork = pwork->next;
                    continue;
                }
                else if (abs(current_track - pwork->id) >= abs(current_track - p->id))
                    pwork = p;
            p = p->next;
        }
        track_process(&current_track, pwork);
        pwork = list->next;
        p = list->next;
        cnt = check(list);
    }
    printf("\n");
    avg_move_track(list);
    free(list);
}

void scan(char *filename)
{ //电梯调度算法
    printf("sstf:");
    track *list;
    list = (track *)malloc(sizeof(track));
    init(list, filename);
    track *p = list->next;
    track *pwork = list->next;
    int current_track = init_track;
    int cnt = 1;
    int direction = 1; //表示磁头移动方向
    while (cnt && p != NULL)
    {
        while (p != NULL)
        { //
            if (p->status != 0)
                if (pwork->status == 0)
                {
                    pwork = pwork->next;
                    continue;
                }
                else if (direction == 1 && p->id >= current_track)
                {
                    if (pwork->id < current_track)
                        pwork = p;
                    else if (pwork->id - current_track > p->id - current_track)
                        pwork = p;
                }

                else if (direction == 0 && p->id <= current_track)
                {
                    if (pwork->id > current_track)
                        pwork = p;
                    else if (current_track - pwork->id > current_track - p->id)
                        pwork = p;
                }
            p = p->next;
        }
        track_process(&current_track, pwork);
        check_direction(list, pwork, &direction);
        pwork = list->next;
        p = list->next;
        cnt = check(list);
    }
    printf("\n");
    avg_move_track(list);
    free(list);
}

int main()
{
    char filename[10] = "test.txt";
    fcfs(filename);
    sstf(filename);
    scan(filename);
    return 0;
}
