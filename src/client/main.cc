#include <iostream>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include "net/net_base.h"

class tcp_connector
{
public:
    tcp_connector()
    :sock_fd_(-1)
    {
    }
    
    bool connect(const char *ipaddr,const short port)
    {
        sock_fd_=net_tcp_connect(ipaddr,port);
        return sock_fd_>0;
    }
    
    ssize_t send(const char *buf,const size_t len)
    {
        return ::send(sock_fd_,buf,len,0);
    }
    
    ssize_t recv(char *buf,const size_t len)
    {
        return ::recv(sock_fd_,buf,len,0);
    }
    
    void close()
    {
        ::close(sock_fd_);
    }
    
private:
    int sock_fd_;
};


int main(int argc, char *argv[])
{
  tcp_connector client;
  if(client.connect("127.0.0.1",6543))
  {
    do{
    char buf[1024]="hello server";
    client.send(buf,strlen(buf)+1);
    std::cout<<"send data..."<<std::endl;
    
    client.recv(buf,sizeof(buf));
    std::cout<<"recv data :"<<buf<<std::endl;
    
    sleep(2);
    }while(1);
    
    client.close();
  }

  return 0;

}