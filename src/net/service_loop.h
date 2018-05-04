#ifndef _NET_REACTOR_H_
#define _NET_REACTOR_H_
#include <map>
#include "base/thread_queue.h"
#include "base/functor.h"
#include "net/tcp_socket_addr.h"

typedef owner::function<void,SocketAddress>  RunableFunctor;

class EventHandler;
class EventDispatcher;
class SocketAddress;
class WakeupHandler;

class ServiceLoop
{
    typedef std::map< int, EventHandler*> handlers_list_t;
    
public:
    ServiceLoop ( );
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

#endif /* REACTOR_H */