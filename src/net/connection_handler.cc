#include "net/connection_handler.h"
#include "net/net_base.h"
#include "net/service_loop.h"
#include <assert.h>
#include <string.h>
#include <algorithm>
#include <stdio.h>

ConnectionHandler::ConnectionHandler()
: EventHandler()
, remote_addr_()
, srv_loop_(NULL)
{
}

ConnectionHandler::~ConnectionHandler()
{
}

void ConnectionHandler::set_service_loop(ServiceLoop *loop)
{
    srv_loop_=loop;
}

void ConnectionHandler::open( SocketAddress remote)
{
    set_remote_addr(remote);
    
    net_set_keepalive(handle());
    net_set_tcp_nodelay(handle());

    buffer_in_.clear();
    buffer_out_.clear();

    this->on_connect(remote);

    int ret = srv_loop_->register_event(handle(), true, false, this);
    assert(0 == ret);
}

void ConnectionHandler::close()
{
    if (handle()>0)
    {
        int ret = srv_loop_->delete_event(handle());
        assert(0 == ret);
    }

    this->on_close();
    
    delete this;
}

void ConnectionHandler::do_handle_read()
{
    int save_errno = 0;
    int ret = buffer_in_.read(handle(), save_errno);
    if (ret <= 0)
    {
        close();
        return;
    }
    else if (ret > 0)
    {
        int len = this->on_read(buffer_in_);
        buffer_in_.ignore(len);
    }
}

void ConnectionHandler::do_handle_write()
{
    std::string msg = buffer_out_.to_string();
    if (msg.size() > 0)
    {
        ssize_t bytes = ::send(handle(), msg.c_str(), msg.size(),0);
        assert(bytes > 0);

        buffer_out_.ignore(bytes);

        if (buffer_out_.empty())
        {
            int ret = srv_loop_->modify_event(handle(), true, false, this);
            assert(0 == ret);
        }
    }
}

void ConnectionHandler::do_handle_error()
{
    close();
}

void ConnectionHandler::send(const char*buf, const size_t len)
{
    bool status = buffer_out_.empty();
    buffer_out_.put(buf, len);

    if (status)
    {
        int ret = srv_loop_->modify_event(handle(), true, true, this);
        assert(0 == ret);
    }
}

void ConnectionHandler::send(const std::string data)
{
    bool status = buffer_out_.empty();
    buffer_out_.put(data);

    if (status)
    {
        int ret = srv_loop_->modify_event(handle(), true, true, this);
        assert(0 == ret);
    }
}