#include <stdio.h>
#include <stdlib.h>
#define MAX 5

typedef struct tag_pcb
{
   char name[8];
   int need; //须运行的时间
   int turn; //周转时间
   struct tag_pcb *next;
} PCB;
PCB *RQ1, *RQ2, *Finish;
int clock = 0; //时钟

void CreateRQ(PCB *RQ, char *filename)
{//从文件读取数据创建链表
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
      fscanf(fp, "%s %d %d", q->name, &q->need, &q->turn);
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
      printf("test:%s\t%d\t%d\n", p->name, p->need, p->turn);
}

void disprocess(PCB *p)
{//打印进程
   printf("RR:\t%s\t%d\t%d\n", p->name, p->need, p->turn);
}

int check(PCB *RQ)
{ //检查队列中进程是否全部运行结束
   PCB *p;
   p = RQ->next;
   while (p != NULL && p->need == 0) //跳过已运行完成的进程
      p = p->next;

   if (p == NULL)
      return 0;
   else
      return 1;
}

void RR(PCB *RQ, int timeslice)
{//时间片轮转算法
   PCB *p; //表示准备运行的进程
   p = RQ->next;
   int count = 1; //表示队列中尚有进程未结束运行
   while (count && p != NULL)
   {

      if (p->need > timeslice)
      { //运行时间大于时间片,进程未完成
         clock += timeslice;
         p->need -= timeslice;
      }
      else if (p->need > 0)
      { //进程完成
         clock += p->need;
         p->turn += clock;
         p->need = 0;
      }
      p = p->next;

      if (p == NULL)
         p = RQ->next; //从头开始遍历
      count = check(RQ);
   }
}

void SJF(PCB *RQ)
{//短进程优先
   PCB *p = RQ->next;
   PCB *pmin = p;
   int count = 1; //表示队列中尚有进程未结束运行
   while (count && p != NULL)
   {
      while (p != NULL)
      { //选出最短进程
         if (p->need != 0 && pmin->need > p->need)
         {
            if (pmin->need == 0)
               pmin = pmin->next;
            else
               pmin = p;
         }
         p = p->next;
      }

      clock += pmin->need;
      pmin->turn += clock;
      pmin->need = 0;
      p = RQ->next;
      pmin = RQ->next;

      count = check(RQ);
   }
}

void runtime(PCB *RQ)
{//队列中进程周转时间
   printf("Turnaround time for process:\n");
   PCB *p = RQ->next;
   while (p != NULL)
   {
      printf("%s\t%d\n", p->name, p->turn);
      p = p->next;
   }
   printf("\n");
}

void avgtime(PCB *R1, PCB *R2)
{//平均周转时间
   printf("Average turnaround time for process:\n");
   PCB *p = R1->next;
   int total_time = 0;
   int pnum = 0;
   for (; p != NULL; p = p->next)
   {
      total_time += p->turn;
      pnum++;
   }
   p = R2->next;
   for (; p != NULL; p = p->next)
   {
      total_time += p->turn;
      pnum++;
   }
   printf("%.1f\n", total_time * 1.0 / pnum);
}

int main()
{
   RQ1 = (PCB *)malloc(sizeof(PCB)); //初始化头节点
   CreateRQ(RQ1, "RQ1.txt");//创建队列
   RQ2 = (PCB *)malloc(sizeof(PCB)); //初始化头节点
   CreateRQ(RQ2, "RQ2.txt");
   //display(RQ1);
   //display(RQ2);
   int timeslice = 7;
   RR(RQ1, timeslice); //轮转法
   SJF(RQ2);           //短进程优先
   runtime(RQ1);       //各进程周转时间
   runtime(RQ2);
   avgtime(RQ1, RQ2); //进程平均周转时间
   printf("Press enter to continue...");
   getchar();

   return 0;
}
