#include<stdio.h>
#include"zmalloc.h"
int main()
{
    int *a = zmalloc(sizeof(int));
    *a = 1;
    printf("%d\n", *a);
    return 0;
}
