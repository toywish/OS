实验：银行家算法
```c
#define  n  5  //进程个数
#define  m  3  //资源种类
int   Available[m],Alloc[n][m],Need[n][m];
main()
{
   int request[m];
   input( );
   while (1)
   {
      read_req( ); 
      if  (请求结束)   break;
 (1) if (!(requesti<=Needi))   表示非法请求；
 (2) if (!(requesti<=Available)) 则Pi阻塞； 
(3)	试探性分配
Available=Available - Requesti;
        Alloci=Alloci+Requesti;
        Needi=Needi-Requesti;
 (4)若新状态安全，则实际分配资源给Pi,否则取消试探性分配。
  }
}
```

```
安全状态判别算法：
(1)设置Finish=(false,...,false)  work=Available
(2)循环查找满足下列条件的进程pi //最多循环n次
    Finish[i]=false且Needi<=work
(3)若找到则Finish[i]=true;work=work+Alloci; 转(2)
(4)若Finish=(true,...,true) 则安全，否则不安全。


测试数据：m=3:种类型的资源（A,B,C,） 进程个数n=5
Available=(2,3,3);     
	      已分配资源数量           资源需求量
	 A        B         C	  A        B         C
P1	 2        1         2	  3        4         7
P2	 4        0         2	  1        3         4
P3	 3        0         5	  0        0         3
P4	 2        0         4	  2        2         1
P5	 3        1         4	  1        1         0
 
请求序列如下：
a.进程P2请求资源（0，3，4）
b.进程P4请求资源（1，0，1）
c.进程P1请求资源（2，0，1）
d.进程P3请求资源（0，0，2） 
```
