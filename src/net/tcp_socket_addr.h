#ifndef TCP_SOCKET_ADDR_H
#define TCP_SOCKET_ADDR_H
#include <sys/types.h>
#include <netinet/in.h>
#include <string>

class SocketAddress
{
public:
    SocketAddress();
    SocketAddress ( const char *ipaddr, unsigned short port );
    SocketAddress ( const int socket_fd, const struct sockaddr addr );
    SocketAddress ( const SocketAddress& other );
    virtual ~SocketAddress ( );
    
    void set_fd(int fd)  { socket_fd_=fd; }
    int get_fd() { return socket_fd_; }
    
    const std::string get_ipaddr ( );
    const std::string get_ipaddr ( ) const;
    unsigned short get_port ( );
    unsigned short get_port ( ) const;
    
    struct sockaddr *get_sockaddr();
    struct sockaddr *get_sockaddr() const;
    
    std::string to_string ( );
    std::string to_string ( ) const;

    SocketAddress& operator=(const SocketAddress& other );
    
private:
    int socket_fd_;
    struct sockaddr_in addr_;
};

#endif /* TCP_SOCKET_H */

