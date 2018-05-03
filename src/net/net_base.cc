#include "net/net_base.h"
#include <sys/types.h>   
#include <sys/socket.h>  //connect,send,recv,setsockopt
#include <netinet/in.h>   // sockaddr_in, "man 7 ip" ,htons   
#include <arpa/inet.h>    //inet_addr,inet_aton
#include <netinet/tcp.h> //TCP_NODELAY
#include <netdb.h>         //gethostbyname
#include <stdlib.h>  
#include <string.h>         // memset
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>

#define LISTEN_QUEUE_SIZE	1024

int net_tcp_bind(const char *ip, unsigned short port)
{
  struct sockaddr_in addr;
  int sock = -1;
  int on = 1;

  if (ip == NULL || ip[0] == 0)
    ip = "0.0.0.0";

  memset(&addr, 0, sizeof (addr));
  addr.sin_family = AF_INET;
  inet_pton(AF_INET, ip, &addr.sin_addr);
  addr.sin_port = htons(port);

  sock = ::socket(PF_INET, SOCK_STREAM, 0);
  if (sock == -1)
    goto error;

  ::setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof (on));

  if (::bind(sock, (struct sockaddr *) &addr, sizeof (addr)) == -1)
    goto error;

  return sock;

error:
  if (sock != -1)
    ::close(sock);

  return -1;
}

int net_tcp_listen(const char *ip, unsigned short port)
{
  int sock = net_tcp_bind(ip, port);
  if (sock >= 0)
  {
    if (::listen(sock, LISTEN_QUEUE_SIZE) == -1)
    {
      ::close(sock);
      return -1;
    }
  }
  return sock;
}

int net_ip_sockaddr(const char *host, unsigned short port, struct sockaddr_in *addr)
{
  int errcode = 0;

  addr->sin_family = AF_INET;
  addr->sin_addr.s_addr = htonl(INADDR_ANY);

  if (host && host[0])
  {
    struct in_addr ipaddr;

    if (inet_aton(host, &ipaddr))
    {
      addr->sin_addr = ipaddr;
    } 
    else if (strcasecmp(host, "localhost") == 0)
    {
      addr->sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    } 
    else
    {
      struct addrinfo hints ={0}, *res = NULL;

      /* Don't use gethostbyname(), it's not thread-safe. 
         Use getaddrinfo() instead.
       */
      hints.ai_family = AF_INET;
      hints.ai_socktype = SOCK_STREAM;
      hints.ai_protocol = IPPROTO_TCP;
      errcode = ::getaddrinfo(host, NULL, &hints, &res);
      if (errcode != 0)
      {
        fprintf(stderr, "getaddrinfo() failed: %s\n", gai_strerror(errcode));
      }
      else
      {
        assert(res->ai_family == AF_INET && res->ai_socktype == SOCK_STREAM);
        assert(res->ai_addrlen == sizeof (*addr));

        memcpy(addr, (struct sockaddr_in *) res->ai_addr, sizeof (*addr));
      }

      if (res)
        ::freeaddrinfo(res);
    }
  }

  addr->sin_port = htons(port);
  return errcode ? -1 : 0;
}

static int _tcp_connect_sockaddr(const struct sockaddr *addr, socklen_t addrlen, int nonblock)
{
  int sock = ::socket(PF_INET, SOCK_STREAM, 0);
  if (sock == -1)
    goto error;

  if (nonblock)
  {
    int flags = ::fcntl(sock, F_GETFL, 0);
    if (flags == -1 || ::fcntl(sock, F_SETFL, flags | O_NONBLOCK) == -1)
      goto error;
  }

  if (::connect(sock, addr, addrlen) == -1)
  {
    if (!nonblock || errno != EINPROGRESS)
      goto error;
  }

  return sock;
  
error:
  if (sock != -1)
    ::close(sock);

  return -1;
}

int net_tcp_connect_sockaddr(const struct sockaddr *addr, socklen_t addrlen)
{
  return _tcp_connect_sockaddr(addr, addrlen, 0);
}

int net_tcp_connect_sockaddr_nonblock(const struct sockaddr *addr, socklen_t addrlen)
{
  return _tcp_connect_sockaddr(addr, addrlen, 1);
}

int net_tcp_connect(const char *host, unsigned short port)
{
  struct sockaddr_in addr;

  if (net_ip_sockaddr(host, port, &addr) < 0)
    return -1;

  return _tcp_connect_sockaddr((struct sockaddr *) &addr, sizeof (addr), 0);
}

int net_tcp_connect_nonblock(const char *host, unsigned short port)
{
  struct sockaddr_in addr;

  if (net_ip_sockaddr(host, port, &addr) < 0)
    return -1;

  return _tcp_connect_sockaddr((struct sockaddr *) &addr, sizeof (addr), 1);
}

int net_set_nonblock(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
		return -1;
	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int net_set_block(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
		return -1;
	return fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);
}

int net_set_tcp_nodelay(int fd)
{
	int flag = 1;
	return setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));
}

int net_set_keepalive(int fd)
{
	int flag = 1;
	return setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &flag, sizeof(flag));
}

int net_set_send_buffer_size(int fd, int size)
{
	return setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &size, sizeof(size));
}

int net_get_send_buffer_size(int fd)
{
	int size;
	socklen_t len = sizeof(size);
	if (getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &size, &len) == 0 && (size_t)len == sizeof(size))
		return size;
	return -1;
}

int net_set_recv_buffer_size(int fd, int size)
{
	return setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));
}

int net_get_recv_buffer_size(int fd)
{
	int size;
	socklen_t len = sizeof(size);
	if (getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &size, &len) == 0 && (size_t)len == sizeof(size))
		return size;
	return -1;
}

int net_set_reuse_address(int fd)
{
	int flag = 1;
	return setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
}

int net_clear_reuse_address(int fd)
{
	int flag = 0;
	return setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
}

