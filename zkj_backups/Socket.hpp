#ifndef __MY_SOCKET__
#define __MY_SOCKET__

#include <iostream>
#include <string>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>

#define DEFAULTIP "127.0.0.1"
#define DEFAULTPORT 8888

namespace cloud
{
    class Socket
    {
    private:
        void MySocket()
        {
            listensock_ = socket(AF_INET, SOCK_STREAM, 0);
            if (listensock_ == -1)
            {
                std::cout << "socket fail ..." << std::endl;
                exit(-1);
            }
        }

    public:
        Socket()
        {
            MySocket();
            port_ = DEFAULTPORT;
            ip_ = DEFAULTIP;
        }
        ~Socket()
        {
            close(listensock_);
        }

    public:
        void MyBind()
        {
            struct sockaddr_in server;
            server.sin_family = AF_INET;
            // inet_aton(ip_.c_str(), &server.sin_addr);
            server.sin_addr.s_addr = INADDR_ANY;
            server.sin_port = htons(port_);
            int n = bind(listensock_, (sockaddr *)&server, sizeof(server));
            if (n < 0)
            {
                std::cout << "bind fail ..." << std::endl;
                exit(-1);
            }
        }

        void MyListen(int backlog = 10)
        {
            int n = listen(listensock_, backlog);
            if (n == -1)
            {
                std::cout << "listen fail ..." << std::endl;
                exit(-1);
            }
        }
        int Accept(uint16_t *clientport, std::string *clientip)
        {
            struct sockaddr_in client;
            socklen_t len = sizeof(client);
            int newfd = accept(listensock_, (struct sockaddr *)&client, &len);
            if (newfd < 0)
            {
                std::cout << "accept fail ..." << std::endl;
                exit(-1);
            }
            char ipbuff[64];
            inet_ntop(AF_INET, &client.sin_addr, ipbuff, sizeof(ipbuff));
            *clientip = ipbuff;
            *clientport = ntohs(client.sin_port);
            return newfd;
        }
        bool Connect(uint16_t serverport, std::string &serverip)
        {
            struct sockaddr_in server;
            memset(&server, 0, sizeof(server));
            server.sin_family = AF_INET;
            server.sin_port = htons(serverport);
            // inet_aton(serverip.c_str(), &server.sin_addr);
            inet_pton(AF_INET, serverip.c_str(), &(server.sin_addr));

            socklen_t len = sizeof(server);
            if (connect(listensock_, (struct sockaddr *)&server, len) < 0)
            {
                std::cout << "connect fail ..." << std::endl;
                return false;
            }
            return true;
        }
        int Fd()
        {
            return listensock_;
        }

    private:
        int listensock_;
        uint16_t port_;
        std::string ip_;
    };
}

#endif