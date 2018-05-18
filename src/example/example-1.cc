// Copyright tang.  All rights reserved.
// https://github.com/tangyibo/libolptnet
//
// Use of this source code is governed by a BSD-style license
//
// Author: tang (inrgihc@126.com)
// Data : 2018/5/2
// Location: beijing , china
/////////////////////////////////////////////////////////////
#include "net/tcp_socket_addr.h"
#include "net/service_loop.h"
#include "net/connection_handler.h"
#include "net/async_acceptor.h"
#include <iostream>

using namespace olptnet;

class session_handler : public ConnectionHandler
{
public:
    virtual void on_connect(const SocketAddress &remote)
    {
        std::cout << "connect:" << remote.to_string() << std::endl;
    }

    virtual int on_read(const input_stream_buffer &input)
    {
         std::cout << input.to_string() << std::endl;
         send(input.to_string());
         
         return input.length();
    }

    virtual void on_close()
    {
         std::cout << "close..." << std::endl;
    }

};

class acceptor_handler : public AsyncAcceptor
{
public:
    acceptor_handler(ServiceLoop *loop):AsyncAcceptor(loop),loop_(loop)
    {
    }
    
    bool on_accept(const SocketAddress &addr)
    {
        ConnectionHandler *ptr = new session_handler();

        ptr->set_service_loop(loop_);
        ptr->open(addr);

        return true;
    }
private:
    ServiceLoop *loop_;
};

int main(int argc, char *argv[])
{
    SocketAddress endpoint("127.0.0.1", 6543);
    ServiceLoop loop;
    acceptor_handler acceptor(&loop);
    
    if (0 == acceptor.open(endpoint))
    {
        loop.run();
    }

    return 0;
}