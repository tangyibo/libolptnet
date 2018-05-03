#ifndef SOCKET_NET_H
#define SOCKET_NET_H
#include <stdint.h>
#include <endian.h>
#include <sys/types.h>   
#include <sys/socket.h>  //connect,send,recv,setsockopt
#include <netinet/in.h>   // sockaddr_in, "man 7 ip" ,htons   
#include <arpa/inet.h>    //inet_addr,inet_aton
#include <netinet/tcp.h> //TCP_NODELAY
#include <netdb.h>         //gethostbyname


//bind & listen
int net_tcp_bind ( const char *ip, unsigned short port );
int net_tcp_listen ( const char *ip, unsigned short port );

//connect
int net_tcp_connect_sockaddr ( const struct sockaddr *addr, socklen_t addrlen );
int net_tcp_connect_sockaddr_nonblock ( const struct sockaddr *addr, socklen_t addrlen );
int net_tcp_connect ( const char *host, unsigned short port );
int net_tcp_connect_nonblock ( const char *host, unsigned short port );

//set or get socket
int net_set_nonblock ( int fd );
int net_set_block ( int fd );
int net_set_tcp_nodelay ( int fd );
int net_set_keepalive ( int fd );
int net_set_send_buffer_size ( int fd, int size );
int net_get_send_buffer_size ( int fd );
int net_set_recv_buffer_size ( int fd, int size );
int net_get_recv_buffer_size ( int fd );
int net_set_reuse_address ( int fd );
int net_clear_reuse_address ( int fd );

//host bytes & network bytes convert


#endif /* SOCKET_NET_H */

