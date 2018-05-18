// Copyright tang.  All rights reserved.
// https://github.com/tangyibo/libolptnet
//
// Use of this source code is governed by a BSD-style license
//
// Author: tang (inrgihc@126.com)
// Data : 2018/5/2
// Location: beijing , china
/////////////////////////////////////////////////////////////
#include "acceptor_server.h"
#include "service_handler.h"
#include "net/service_loop.h"
#include <iostream>

acceptor_server::acceptor_server(ServiceLoop* loop) 
: AsyncAcceptor(loop)
,thpool_(loop)
{
}

void acceptor_server::start_thread_pool(const int thread_num)
{
    thpool_.set_thread_num(thread_num);
    thpool_.open();
}

void acceptor_server::stop_thread_pool()
{
    thpool_.close();
}

bool acceptor_server::on_accept(const SocketAddress &addr)
{
    if (clients_.size() >= MAX_CLIENT_COUNT)
        return false;

    service_handler *ptr = new service_handler();
    ptr->set_acceptor(this);
    
    ServiceLoop* loop=thpool_.get_loop();
    ptr->set_service_loop(loop);
    
    //ptr->open(addr);
    RunableFunctor f=owner::bind<service_handler,void,SocketAddress>(&service_handler::open,ptr,addr);
    loop->add_run(f);
    
    clients_.push_back(ptr);
    std::cout << "handle_accept..." << std::endl;

    return true;
}

void acceptor_server::remove_handler(const service_handler *handler)
{
    std::vector<service_handler*>::iterator it;
    for (it = clients_.begin(); it != clients_.end(); ++it)
    {
        if (*it == handler)
        {
            clients_.erase(it);
            break;
        }
    }
}

