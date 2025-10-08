#include<cstdio>
#include<sys/types.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<unistd.h>
#include<cstdio>



int main()
{
    
    int s = dup();
    printf("s=%d\n",s);
    return 0;
}