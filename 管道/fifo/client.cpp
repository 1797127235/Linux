#include"namedPipe.hpp"

int main()
{
    Pipe pipe(PATH,USER);
    if(pipe.OpenForWrite())
    {
        while(1)
        {
            std::string msg;
            std::cout << "Please Enter# ";
            std::getline(std::cin,msg);
            if(msg == "quit")
            {
                break;
            }
            int n = pipe.WritePipe(msg);
            if(n > 0)
            {
                std::cout << "write " << n << " bytes msg" << std::endl;
            }
            else 
            {
                std::cerr << "write error" << std::endl;
                break;
            }
        }
    }
}