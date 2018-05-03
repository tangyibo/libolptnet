#include "tcp_socket_addr.h"
#include <netdb.h>
#include <arpa/inet.h> 
#include <string.h>
#include <stdio.h>

SocketAddress::SocketAddress()
:socket_fd_(-1)
{
    bzero(&addr_, sizeof (addr_));
}

SocketAddress::SocketAddress (const int socket_fd,const struct sockaddr addr )
:socket_fd_(socket_fd)
{
    memcpy(&addr_,&addr,sizeof(struct sockaddr_in));
}

SocketAddress::SocketAddress ( const char *ipaddr, unsigned short port )
:socket_fd_(-1)
{
    bzero(&addr_, sizeof (addr_));
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons((short) port);
    inet_pton(AF_INET, ipaddr, &addr_.sin_addr);
}

SocketAddress::SocketAddress ( const SocketAddress& other )
:socket_fd_(other.socket_fd_)
{
    memcpy(&addr_,&other.addr_,sizeof(struct sockaddr_in));
}

SocketAddress::~SocketAddress ( )
{
    
}

SocketAddress& SocketAddress::operator=(const SocketAddress& other)
{
    if (this != &other)
    {
        this->socket_fd_ = other.socket_fd_;
        memcpy(&addr_, &other.addr_, sizeof (struct sockaddr_in));
    }

    return *this;
}

const std::string SocketAddress::get_ipaddr()
{
    char str[16];  
    return inet_ntop(AF_INET, &addr_.sin_addr, str, sizeof(str));  
}

const std::string SocketAddress::get_ipaddr() const
{
    char str[16];  
    return inet_ntop(AF_INET, &addr_.sin_addr, str, sizeof(str));  
}

unsigned short SocketAddress::get_port()
{
    return ntohs(addr_.sin_port);
}

unsigned short SocketAddress::get_port() const
{
    return ntohs(addr_.sin_port);
}

struct sockaddr *SocketAddress::get_sockaddr()
{
    return (struct sockaddr *) (&addr_);
}

struct sockaddr *SocketAddress::get_sockaddr() const
{
    return (struct sockaddr *) (&addr_);
}

std::string SocketAddress::to_string()
{
    char buf[32];
    sprintf(buf,"%s:%d",get_ipaddr().c_str(),get_port());
    return std::string(buf);
}

std::string SocketAddress::to_string() const
{
    char buf[32];
    sprintf(buf,"%s:%d",get_ipaddr().c_str(),get_port());
    return std::string(buf);
}