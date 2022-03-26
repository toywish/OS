#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define maxsize 512
//内存链表
typedef struct rd
{
    int flag;        // 1 被占用，0 空闲
    int size;        //分区大小
    struct rd *next; //下一个分区的地址
} rd_list;
//存储用户请求
typedef struct request
{
    char name[8];     //进程名称
    int request_size; //申请内存大小
    struct request *next;
} request_list;
//记录内存中的进程
typedef struct pcb
{
    char name[8]; //进程名称
    int addr;     //进程地址
    struct pcb *next;
} pcb_list;

//初始化内存
void init_rd(rd_list *t)
{
    rd_list *q = (rd_list *)malloc(sizeof(rd_list));
    q->flag = 0;
    q->size = maxsize;
    q->next = NULL;
    t->next = q;
    printf("内存初始化成功\t初始内存大小：%d\n", t->next->size);
}
//打印内存链表
void print_rd(rd_list *t)
{
    printf("*******************当前内存使用情况*******************\n");
    rd_list *p = t->next;
    for (p; p != NULL; p = p->next)
    {
        if (p->flag)
        {
            printf("已使用\t");
        }
        else
        {
            printf("未使用\t");
        }
        printf("%d\n", p->size);
    }
    printf("*****************************************************\n");
    printf("\n");
}
// 打印用户请求链表
void print_user_request(request_list *t)
{
    request_list *p = t->next;
    for (p; p != NULL; p = p->next)
    {
        printf("%s\t%d\n", p->name, p->request_size);
    }
}
// 打印内存中进程链表
void print_pcb_list(pcb_list *t)
{
    printf("当前内存进程\n");
    pcb_list *p = t->next;
    for (p; p != NULL; p = p->next)
    {
        printf("%s\t%d\n", p->name, p->addr);
    }
    printf("\n");
}

// 分配内存
void request(rd_list *result, int request_size)
{
    //分配内存，如果大小刚好，则仅需修改 flag
    if (result->size == request_size + 1)
    {
        result->flag = 1;
    }
    //分配内存，如果大小刚好，则需修改 flag ,且新建结点
    else if (result->size > request_size + 1)
    {
        //修改当前空闲的内存块大小
        result->size -= request_size + 1;
        //新建使用中的内存块链表节点，并插入到 p 节点后
        rd_list *rd_temp = (rd_list *)malloc(sizeof(rd_list));
        rd_temp->flag = 1;
        rd_temp->size = request_size + 1;
        rd_temp->next = result->next;
        result->next = rd_temp;
    }
}
//释放内存
void Release(rd_list *rd, int addr)
{
    rd_list *pre = rd;//前指针
    rd_list *p = rd->next;
    int size_count = 0;
    for (size_count; size_count <= maxsize; size_count += p->size, p = p->next, pre = pre->next)
    {
        if (p->flag == 1 && size_count + 1 == addr)
        {
            // 上邻空闲，需要合并
            if (pre != rd && pre->flag == 0) //不为头节点，且空闲
            {
                if (p->next != NULL && p->next->flag == 0)
                { //下邻空闲，合并 p 和下邻结点，释放 p 和下邻结点
                    pre->size += p->size + p->next->size;
                    pre->next = p->next->next;
                    free(p->next);
                    free(p);
                }
                else
                { //下邻不空闲，合并 p ，释放 p 结点
                    pre->size += p->size;
                    pre->next = p->next;
                    free(p);
                }
            }
            else
            { //上邻不空闲
                if (p->next != NULL && p->next->flag == 0)
                { //下邻空闲，需将 flag 改为 0 ，下邻合并，释放下邻结点
                    p->flag = 0;
                    rd_list *t = p->next;
                    p->size += t->size;
                    p->next = t->next;
                    free(t);
                }
                else
                { //下邻不空闲，仅需将 flag 改为 0
                    p->flag = 0;
                }
            }
            break;
        }
    }
}

//首次适应，从空闲分区表的第一个表目起查找该表，把最先能够满足要求的空闲区分配给作业，这种方法的目的在于减少查找时间。
int first_time(rd_list *rd, int request_size)
{

    rd_list *p = rd->next;
    int addr = 0;
    for (p; p != NULL; p = p->next)
    {
        addr += p->size;
        if (!p->flag && p->size > request_size)
        {
            addr -= request_size;
            request(p, request_size);
            break;
        }
    }
    if (p != NULL)
    {
        return addr;
    }
    else
    {
        return 0;
    }
}

//最佳适应，从全部空闲区中找出能满足作业要求的，且大小最小的空闲分区，这种方法能使碎片尽量小。
int best(rd_list *rd, int request_size)
{
    rd_list *p = rd->next;
    int addr = 0;
    rd_list *result = NULL; //记录最终选取的结果结点
    int result_addr = 0;
    for (p; p != NULL; p = p->next)
    {
        addr += p->size;
        //内存未使用，内存大小 > 申请的内存
        if (!p->flag && p->size > request_size)
        {
            // 如果还结果结点还未选，或者结果结点的 size > 当前结点
            if (!result || result && result->size > p->size)
            {
                result = p;
                result_addr = addr;
            }
        }
    }
    if (result)
    {
        result_addr -= request_size;
        request(result, request_size);
        return result_addr;
    }
    else
    {
        return 0;
    }
}
//最坏适应，从全部空闲区中找出能满足作业要求的、且大小最大的空闲分区，从而使链表中的节点大小趋于均匀。
int worst(rd_list *rd, int request_size)
{
    rd_list *p = rd->next;
    int addr = 0;
    rd_list *result = NULL; //记录最终选取的结果结点
    int result_addr = 0;
    for (p; p != NULL; p = p->next)
    {
        addr += p->size;
        //内存未使用，内存大小 > 申请的内存
        if (!p->flag && p->size > request_size)
        {
            // 如果还结果结点还未选，或者结果结点的 size < 当前结点
            if (!result || result && result->size < p->size)
            {
                result = p;
                result_addr = addr;
            }
        }
    }
    if (result)
    {
        result_addr -= request_size;
        request(result, request_size);
        return result_addr;
    }
    else
    {
        return 0;
    }
}

//从文件读取用户请求，生成请求链表
void get_user_request(request_list *t, char *filename)
{
    printf("读取用户请求\n");
    FILE *fp;
    if ((fp = fopen(filename, "r")) == NULL)
    {
        printf("Can't open file %s\n", filename);
        exit(0);
    }

    request_list *p = t;
    while (!feof(fp))
    {
        request_list *q = (request_list *)malloc(sizeof(request_list));
        fscanf(fp, "%s %d", q->name, &q->request_size);
        q->next = NULL;
        p->next = q;
        p = q;
    }
    p->next = NULL;
    fclose(fp);
}

//执行用户请求
void run_user_request(int flag, request_list *req_list, rd_list *rd)
{
    request_list *req = req_list->next;
    pcb_list *pcb = (pcb_list *)malloc(sizeof(pcb_list));
    pcb->next = NULL;
    for (req; req != NULL; req = req->next)
    {
        printf("%s\t", req->name);
        if (req->request_size)
        { //申请内存
            printf("申请\t%d\n", req->request_size);
            int addr = 0;
            switch (flag)
            {
            case 1:
                addr = first_time(rd, req->request_size);
                break;
            case 2:
                addr = best(rd, req->request_size);
                break;
            case 3:
                addr = worst(rd, req->request_size);
                break;
            default:
                break;
            }
            //如果申请内存成功，将进程加入到进程链表中
            if (addr)
            {
                printf("内存分配成功\n");
                // 将进程加入进程链表
                pcb_list *pcb_temp = (pcb_list *)malloc(sizeof(pcb_list));
                strcpy(pcb_temp->name, req->name);
                pcb_temp->addr = addr;
                pcb_temp->next = NULL;
                // printf("pcb_temp:%s\t%d\n", pcb_temp->name, pcb_temp->addr);
                pcb_list *q = pcb;
                for (q; q != NULL && q->next != NULL; q = q->next)
                {
                    // printf("%s %d\n", q->name, q->addr);
                }
                q->next = pcb_temp;
            }
            else
            {
                printf("内存分配失败\n");
            }
        }
        else
        { //释放内存
            printf("完成\n");
            //获取进程地址
            pcb_list *p = pcb->next;
            pcb_list *pre = pcb;
            for (p; p != NULL; p = p->next, pre = pre->next)
            {
                if (!strcmp(p->name, req->name))
                {
                    Release(rd, p->addr);
                    // 释放结点
                    pre->next = p->next;
                    free(p);
                    break;
                }
            }
        }
        print_rd(rd);
        //print_pcb_list(pcb);
    }
}

// 获取用户选择的策略
int get_user_choose()
{
    printf("请输入放置策略：[1]首次 [2]最佳 [3]最坏\n");
    int flag = 0;
    int is_right = 0;
    do
    {
        scanf("%d", &flag);
        if (flag == 1 || flag == 2 || flag == 3)
        {
            is_right = 1;
        }
        else
        {
            is_right = 0;
            printf("输入不正确！请重新输入\n");
            printf("请输入放置策略：[1]首次 [2]最佳 [3]最坏\n");
        }
    } while (!is_right);
    switch (flag)
    {
    case 1:
        printf("首次\n");
        break;
    case 2:
        printf("最佳\n");
        break;
    case 3:
        printf("最坏\n");
        break;
    default:
        break;
    }
    return flag;
}
int main()
{
    //获取用户合法输入
    //int flag = get_user_choose();
    int flag = 3;
    //初始化内存
    rd_list *rd = (rd_list *)malloc(sizeof(rd_list));
    init_rd(rd);
    // printf("内存初始化成功\t初始内存大小：%d\n", rd->next->size);
    request_list *data = (request_list *)malloc(sizeof(request_list));
    get_user_request(data, "./list.txt");
    // print_user_request(data);
    run_user_request(flag, data, rd);
    free(rd);
    return 0;
}