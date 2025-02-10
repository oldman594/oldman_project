#include <iostream>
#include <vector>
#include <memory>
#include <pthread.h>
#include "Socket.hpp"

class Pthread_Date
{
public:
    Pthread_Date(int fd,std::string clientip,uint16_t clientport) 
    : _fd(fd),_clientip(clientip),_clientport(clientport) 
    {}
    ~Pthread_Date() {}

public:
    int Fd() { return _fd; }

private:
    int _fd;
    std::string _clientip;
    uint16_t _clientport;
};

void *pthread_run(void *arg)
{
    Pthread_Date *self_fd = static_cast<Pthread_Date *>(arg);
    for (;;)
    {
        char buffer[1024];
        int n = read(self_fd->Fd(), buffer, sizeof(buffer)); // 会阻塞
        buffer[n] = 0;
        std::cout << buffer << std::endl;
    }
}

int main()
{
    cloud::Socket server;
    std::vector<Pthread_Date> pthreads;
    // pthreads.resize(10);
    server.MyBind(); // 绑定端口
    // std::cout << "block Listen" << std::endl;
    server.MyListen(); // 设置监听状态

    // std::cout << "accept Listen" << std::endl;
    // int newfd = server.Accept(&clientport, &clientip);
    for (;;)
    {
        std::string clientip;
        uint16_t clientport;
        int fd = server.Accept(&clientport,&clientip);
        
        if (fd > 0)
        {
            pthread_t t;
            Pthread_Date self_fd(fd,clientip,clientport);
            pthreads.push_back(self_fd);
            pthread_create(&t, NULL, pthread_run, &self_fd);
        }

        //     std::string str = buffer;
        //     auto pos = str.find("GET",0);
        //     if(pos == std::string::npos)
        //     {
        //         std::cout<<"request not standardized"<<std::endl;
        //         return -1;
        //     }
        //     std::cout<<str<<std::endl;
        //     for(auto i = pos;i<str.find("\n",0);i++)
        //         std::cout<<str[i];
        // std::string buffer2 = R"(<!DOCTYPE html><html><head><meta charset="UTF-8"><title>页面标题</title></head><body><h1>我的第一个标题</h1><p>我的第一个段落。</p></body></html>)";
        // write(newfd, buffer2.c_str(), buffer2.size());
    }

    return 0;
}