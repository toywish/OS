#include <stdio.h>
#define n 5 //进程个数
#define m 3 //资源种类

int Available[m] = {2, 3, 3}; //当前剩余资源
int Alloc[n][m] = {{2, 1, 2},
                   {4, 0, 2},
                   {3, 0, 5},
                   {2, 0, 4},
                   {3, 1, 4}}; //已分配资源
int Need[n][m] = {{3, 4, 7},
                  {1, 3, 4},
                  {0, 0, 3},
                  {2, 2, 1},
                  {1, 1, 0}}; //资源需求量

int request_req(int *i, int *request)
{ //读取数据
   printf("Input process number:");
   scanf("%d", i); //此处为 i
   if (*i < 1)     //数据不合法则退出循环
      return 0;
   printf("Input resource request:");
   scanf("%d %d %d", &request[0], &request[1], &request[2]);
   return 1;
}

int judge(int i, int *request)
{//判断请求是否合法
   if (i > n)
   {
      printf("Invalid request!\n");
      return 0;
   }

   int k = 0;
   for (k; k < m; k++)
   {
      if (request[k] > Available[k])
      {
         printf("Process block!\n");
         return 0;
      }

      if (request[k] > Need[i][k])
      {
         printf("Illegal request!\n");
         return 0;
      }
   }

   return 1;
}

void security(int i, int *request)
{//判断安全性
   int k = 0;
   for (k = 0; k < m; k++)
   { //尝试分配
      Available[k] -= request[k];
      Alloc[i][k] += request[k];
      Need[i][k] -= request[k];
   }
   int finish[n] = {0, 0, 0, 0, 0};
   int work[m];
   for (k = 0; k < m; k++)
      work[k] = Available[k];
   int p = 0; //表示进程
   int pnum = 0;
   for (k = 0; p < n && pnum < n;)
   {
      if (finish[p] == 0)
         for (k = 0; k < m; k++)
            if (Need[p][k] > work[k])
               break; //循环中断表示不符合条件
      if (k == m)
      {
         finish[p] = 1;
         int c = 0;
         for (c = 0; c < m; c++)
            work[c] += Alloc[p][c];
         p = 0;
         pnum++;
      }
      else
         p++;
   }

   if (pnum < n)
   { //不安全
      printf("Process block!\n");
      for (k = 0; k < m; k++)
      { //取消分配
         Available[k] += request[k];
         Alloc[i][k] -= request[k];
         Need[i][k] += request[k];
      }
   }
   else
      printf("Allows allocation of resources!!!\n");
}

int main()
{
   int i = 0;
   int request[m] = {0, 0, 0};
   while (1)
   {
      if (request_req(&i, request) == 0)
         break;//输入数据不合法则退出循环

      if (judge(i - 1, request))
         security(i - 1, request);
   }
   printf("Press enter to continue...");
   getchar();
   return 0;
}
