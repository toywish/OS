多级队列调度算法

设RQ分为RQ1和RQ2，RQ1采用轮转法，时间片q=7.
        RQ1>RQ2，RQ2采用短进程优先调度算法。

测试数据如下：RQ1: P1-P5, RQ2: P6-P10　

| 进程 | P1 | P2 | P3 | P4 | P5 |	P6 | P7 | P8 | P9 |	P10 |
| ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- |
|运行时间 |16 |11| 14 |13| 15 |21 |18 |10| 7 | 14|
|已等待时间 |6|5|4|3|2|1|2|3|4|5|

实现描述：

```c
 typedef  struct  tag_pcb
    {  char  name[8];
       int need;//须运行的时间
       int turn;//周转时间
       struct  tag_pcb  *next;
   } PCB;
PCB  *RQ1,*RQ2，*Finish;
int clock=0;  //时钟

int main ( )
{  输入RQ1；
   输入RQ2；（最好从文件读入）
   while（ＲＱ１！＝ＮＵＬＬ）
{    从RQ1中选取一进程Pi准备运行；
      计算其运行的时间t;
      clock+=t;  //表示Pi运行t;
      if (Pi完成）  计算其turn;
      否则   Pi加入到队尾；
   }
   while（ＲＱ2!＝ＮＵＬＬ）
   { 从RQ2中选取一进程Pi准备运行；   
      clock+=Pi.need;
      计算Pi的turn;
   }
   输出进程的周转时间；   
}
　
```
