#include"namedPipe.hpp"


int main()
{
    Pipe pipe(PATH,CREATER);
    if(pipe.OpenForRead() )
    {
        while(1)
        {
            std::string msg;
            int n = pipe.ReadPipe(&msg);
            if(n > 0)
            {
                std::cout << "read " << n << " bytes msg : " << msg << std::endl;
            }
            else if(n == 0)
            {
                std::cout << "pipe closed" << std::endl;
                break;
            }
            else 
            {
                std::cerr << "read error" << std::endl;
                break;
            }
        }
    }

}