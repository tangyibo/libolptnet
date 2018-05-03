#ifndef _ACCEPTOR_HANCLER_H_
#define _ACCEPTOR_HANCLER_H_
#include "net/event_handler.h"

class ServiceLoop;
class SocketAddress;

class AsyncAcceptor : public EventHandler
{
public:
    AsyncAcceptor (ServiceLoop *loop);
    virtual ~AsyncAcceptor ( );

    int open (const SocketAddress *local_addr );
    void close();

    virtual int handle ( ) {   return listen_fd_;   }

    virtual void do_handle_read ( );
    virtual void do_handle_write ( );
    virtual void do_handle_error ( );
    
    virtual bool on_accept(const SocketAddress &addr);

protected:
    int listen_fd_;
    ServiceLoop *srv_loop_;
};

#endif /* TCP_LISTENER_H */

