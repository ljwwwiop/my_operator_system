#include <stdio.h>

#include <iostream>


int add_range(int low, int high)
{
    int i,sum;
    for(i = low;i<=high;i++)
        sum = sum + i;
    return sum;
}

// 复习位运算
/*
    按位与 &,都为1才为1
    0&0 = 0, 0&1 = 0,1&0 = 0, 1&1 = 1
    按位或 |,一个为1就为1，两个0就是0
    0&0 = 0, 0&1 = 0,1&0 = 0, 1&1 = 1
    按位异或^ 运算时如果两个数相同，则返回0，如果不相同则返回1
    0^0=0，0^1=1，1^0=1，1^1=0
    取反 ~
    ~1 = 0, ~0 = 1
    左移<< 运算时将这个数中的所有1向左移，空出的位补上0
    右移>> 运算时将这个数中的所有1向右移，空出的位补符号

    负数 = 补码 表示
    绝对值 = 取反 + 1 = 还原了

    取两个数的平均数，可以有效的防止数据溢出或者越界
    (a&b) + ((a^b) >> 1)

    取int最值
    (1<<31)-1;//int的最大值
    (1<<31);//int的最小值
    ((long long)1<<63)-1;//long long的最大值
    ((long long)1<<63);//long long的最小值

    n&1 最后一位判断奇偶

    取出该数二进制对应的m位置
*/

int main()
{
    unsigned int a = 5,b = 9;
    short c = -2;
    short d ,e;
    d = c>>31;
    d = (d^c) - d; // 取到了绝对值

    printf("%d\n",a&b);   
    printf("%d\n",a|b);   
    printf("%d\n",a^b); 
    printf("%d\n",~a); 
    printf("%d\n",~b); 

    printf("%d\n",a<<1);
    printf("%d\n",a>>2);

    printf("%d\n",c>>1);

    e = (b>>(3-1))&1;
    printf("%d\n",e);

    e = b|(1<<(3-1));
    printf("%d\n",e);

    e = b&~(1<<(4-1));
    printf("%d\n",e);
    
    return 0;
}


