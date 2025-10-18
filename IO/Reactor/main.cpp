#include"Reactor.hpp"
#include"Listener.hpp"
#include"HandlerConnection.hpp"
#include"PackageParse.hpp"
#include<memory>


int main(int argc,char* argv[])
{
    if(argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        return 1;
    }

    uint16_t port = std::atoi(argv[1]);
    InetAddr localaddr("0.0.0.0",port);

    //处理普通sockfd的模块
    PackageParse parser;
    HandlerConnection handlers(std::bind(&PackageParse::Excute, &parser, std::placeholders::_1));
    
    Reactor reactor;
    Listener listener(port);
    
    //为Reactor设置处理sockfd的回调（必须在AddConnection之前）
    reactor.SetOnConnect(std::bind(&Listener::Acceptor,&listener,std::placeholders::_1));
    reactor.SetOnNormalHandler(
        std::bind(&HandlerConnection::HandleRecver, &handlers, std::placeholders::_1),
        std::bind(&HandlerConnection::HandleSender, &handlers, std::placeholders::_1),
        std::bind(&HandlerConnection::HandleExcepter, &handlers, std::placeholders::_1)
    );
    

    //必须先初始化好服务器再加入
    reactor.AddConnection(listener.listensock(),EPOLLIN|EPOLLET,localaddr,ConnectionType::ListenConnection);

    reactor.Dispatcher();

    return 0;
}