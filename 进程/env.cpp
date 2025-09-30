#include<iostream>
#include<cstring>
#include<unistd.h>

int main(int argc, char* argv[], char* env[])
{
    // for(int i = 0; env[i] != nullptr; ++i)
    // {
    //     printf("env[%d]->%s\n", i, env[i]);
    // }
    char* path = getenv("PATH");
    if(path != nullptr)
    {
        printf("PATH->%s\n", path);
    }
    return 0;
}