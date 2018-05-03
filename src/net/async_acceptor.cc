#include "net/async_acceptor.h"
#include "net/connection_handler.h"
#include "net/service_loop.h"
#include "net/net_base.h"
#include <assert.h>
#include <errno.h>

AsyncAcceptor::AsyncAcceptor(ServiceLoop *loop)
:EventHandler()
,listen_fd_(-1)
,srv_loop_(loop)
{
}

AsyncAcceptor::~AsyncAcceptor()
{
    close();
}

int AsyncAcceptor::open(const SocketAddress *local_addr)
{
    const char *ipaddr = local_addr->get_ipaddr().c_str();
    unsigned short port = local_addr->get_port();
    listen_fd_ = net_tcp_listen(ipaddr, port);
    if (listen_fd_ < 0)
        return -1;
  
  SocketAddress* tmp=const_cast<SocketAddress *>(local_addr);
  tmp->set_fd(listen_fd_);
  
  net_set_reuse_address(listen_fd_);
  
  int result=srv_loop_->register_event(handle(), true, false,this);
  assert(0==result);
  return result;
}

void AsyncAcceptor::close()
{
    if(handle()>0)
    {
        int result = srv_loop_->delete_event(handle());
        assert(0 == result);
    }
  
    if(listen_fd_>0)
    {
        ::close(listen_fd_);
        listen_fd_=-1;
    }
}

void  AsyncAcceptor::do_handle_read()
{
    SocketAddress sockaddr;
    socklen_t addrlen = sizeof (struct sockaddr);
    int fd = ::accept(listen_fd_, sockaddr.get_sockaddr(), &addrlen);
    if (fd > 0)
    {
        sockaddr.set_fd(fd);
        if (!this->on_accept(sockaddr))
        {
            ::close(fd);
            return;
        }

        net_set_tcp_nodelay(fd);
    }
}

void AsyncAcceptor::do_handle_write()
{
    //nothing
}

void AsyncAcceptor::do_handle_error()
{
    //nothing
}

bool AsyncAcceptor::on_accept(const SocketAddress &addr)
{
//    ConnectionHandler *ptr = new ConnectionHandler();
//
//    ptr->set_service_loop(loop);
//    ptr->open(addr);
//    
    return true;
}