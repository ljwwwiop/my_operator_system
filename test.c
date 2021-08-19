#include <stdio.h>

/* 
    gcc,g++ ,g++ -g hello.cpp -o hello
    -fno-pie -no-pie 4.8以后动态连接库，默认开启的。
    file test
    objdump -d test | less 代码反转到汇编
    nm test
    一个字节 8bit
    char 一个字节 short 两字节 int 四字节
    gdb a.out 调试debug 
    start  开始命令 starti 第一条指令 x/i x/x $rip 打印rip指令的二进制数值
    info inferiors 查看当前少数进程的pid号 
    x 0x40000  访问内存
    layout asm 换一种方式调试汇编代码 si 继续下一跳
    echo $? 查看返回值
    ld xx.o 链接汇编
    man 手册 - 永远嘀神
    man 2 syscall 
    man 2 write

    ## 好东西
    vi.c from busybox
    telnet towel.blinkenlights.nl
    dialog --msgbox 'Hello OS world!' 8 32

    # 解析二进制文件

    strace 工具 追踪程序执行过程
    strace ./a
    strace ./a |& less
    strace ./a |& vim -
    strace -f gcc a.c系统调用

    threads 线程并发

    # gdb 中线程调试工具
    set scheduler-locking on
    info threads 
    thread id 

*/

void s(int (*arr)[4],int n){
    int sum = 0;
    for(int i = 0;i<4;i++)
        printf("%d -",*(*(arr + 1) + i));
    printf("\n");
    // sum/=4;
    // printf("%d\n",sum);
}

void s2(int (*arr)[4],int n){
    int sum = 0;
    for(int i = 0;i<4;i++)
        printf("%d -",*(*(arr + 1) + i));
    printf("\n");
    // sum/=4;
    // printf("%d\n",sum);
}


int main()
{
    int array[3][4] = {{89,90,92,100},{67,23,56,48},{78,69,90,90}};
    s2(array ,1);

    // int (*p)[4];
    // p = array;
    // for(int i = 0; i< 3;i++)
    //     for(int j = 0;j<4;j++)
    //         printf("%d - ",*(*(p + i) + j));

    // s(array,1);
    // printf("%p",array);

    // 二级指针
    // int **p2,*q;
    // for(int i=0;i<3;i++)
    // {
    //     q = *(array + i);
    //     p2 = &q;
    //     for(int j = 0;j<4;j++)
    //         printf("%d",*(*p2 + j));
    // }


    // printf("main %p\n",main); 0x7ffd2a1423708990921006723564878699090 
    // int a = 0,b = 0;
    // printf("%p %p\n",&a,&b);
    // printf("%d\n",&b - &a);
    // printf("%d\n",(__uint8_t*)&b - (__uint8_t*)&a);
    return 0;

}



