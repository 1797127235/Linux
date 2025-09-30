#include<stdio.h>
#include<cstdlib>
#include<unistd.h>


int main()
{
    int n;
    printf("pid = %d\n", getpid());
    scanf("%d", &n);
}