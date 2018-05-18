// Copyright tang.  All rights reserved.
// https://github.com/tangyibo/libolptnet
//
// Use of this source code is governed by a BSD-style license
//
// Author: tang (inrgihc@126.com)
// Data : 2018/5/2
// Location: beijing , china
/////////////////////////////////////////////////////////////
#ifndef _NET_REACTOR_H_
#define _NET_REACTOR_H_
#include <map>
#include "base/thread_queue.h"
#include "base/functor.h"
#include "net/tcp_socket_addr.h"

namespace olptnet{
    
//0-select;1-poll;2-epoll
#define EVENT_POLL_DISPATCHER_METHOD 2

typedef owner::function<void,SocketAddress>  RunableFunctor;

class EventHandler;
class EventDispatcher;
class SocketAddress;
class WakeupHandler;

class ServiceLoop
{
    typedef std::map< int, EventHandler*> handlers_list_t;
    
public:
    ServiceLoop (const int poll=EVENT_POLL_DISPATCHER_METHOD);
    virtual ~ServiceLoop ( );

    void add_run ( RunableFunctor &f );
    void wakeup_event ( );
    void run ( );
    
    void quit();
    
    int register_event ( const int h, bool readable, bool writeable, EventHandler *handler );
    int modify_event ( const int h, bool readable, bool writeable, EventHandler *handler );
    int delete_event ( const int h );

private:
    ServiceLoop ( const ServiceLoop& loop );

private:
    bool quit_;
    EventDispatcher* dispatcher_;
    handlers_list_t handlers_;
    ThreadQueue< RunableFunctor > functors_;
    WakeupHandler *wakeup_;
};

} //end namespace olptnet

#endif /* REACTOR_H */