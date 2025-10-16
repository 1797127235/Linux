#include<iostream>
#include<fcntl.h>
#include<errno.h>
#include<unistd.h>

int main()
{
    char buffer[1204];
    int fl = fcntl(0, F_GETFL);
    if(fcntl(0, F_SETFL, fl | O_NONBLOCK) == -1)
    {
        std::cout << "fcntl error" << std::endl;
        return -1;
    }
    while(true) 
    {
        int n = read(0, buffer,sizeof(buffer)-1);
        if(n > 0)
        {
            buffer[n] = '\0';
            std::cout << "Read " << n << " bytes" << std::endl;
            std::cout << buffer << std::endl;
            break;
        }
        else if(n == 0)
        {
            std::cout << "End of file" << std::endl;
            break;
        }
        else
        {
            if(errno == EAGAIN)
            {
                std::cout << "No data available" << std::endl;
                sleep(1);
                continue;
            }

            if(errno == EINTR)
            {
                std::cout << "Interrupted by signal" << std::endl;
                continue;
            }

            std::cout << "Error reading from stdin" << std::endl;
            break;
        }
    }

    return 0;

}