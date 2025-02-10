#include "Socket.hpp"

void number_warn()
{
    std::cout << "./client 127.0.0.1 8888" << std::endl;
    exit(-1);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
        number_warn();
    cloud::Socket sock;
    std::string serverip = argv[1];
    uint16_t serverport = std::stoi(argv[2]);
    sock.Connect(serverport, serverip);
    int newfd = sock.Fd();
    while (1)
    {
        int n = 0;
        int cnt = 5;
        bool Identifier = false;
        do
        {
            n = sock.Connect(serverport, serverip);
            if (n < 0)
            {
                cnt--;
                Identifier = true;
            }
            else
            {
                Identifier = false;
                break;
            }
        } while (cnt-- && Identifier);
        if(cnt == 0 || Identifier == true)
            break;
        
        std::string buffer;
        // std::string str;
        // std::cin>>str;buffer+=str;
        std::getline(std::cin, buffer);
        // buffer += "client# ";
        // buffer += "connect success";
        write(newfd, buffer.c_str(), buffer.size());
    }
    return 0;
}