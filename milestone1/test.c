#include <stdio.h>

int mod(int a, int b)
{
    while(a >= b)
        a = a - b;
    return a;
}

int div(int a, int b)
{
    int q = 0;
    while(q*b <= a)
        q = q+1;
    return q-1;
}

int main()
{

    printf("%d\n",mod(7, 4));
    printf("%d\n",mod(13, 4));
    printf("%d\n",mod(19, 4));
    printf("%d\n",mod(50, 4));
    printf("%d\n",mod(4, 4));
    printf("%d\n",mod(1, 4));
    printf("%d\n",div(7, 4));
    printf("%d\n",div(4, 4));
    printf("%d\n",div(10, 4));
    printf("%d\n",div(19, 4));
    return 0;
}

