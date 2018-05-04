#ifndef SERVICE_ACCETOR_H
#define SERVICE_ACCETOR_H
#include "net/tcp_socket_addr.h"
#include "net/async_acceptor.h"
#include "net/thpool_service_loop.h"
#include <vector>

#define MAX_CLIENT_COUNT    1024

class service_handler;
class acceptor_server : public AsyncAcceptor
{
public:
    acceptor_server(ServiceLoop* loop) ;

    void start_thread_pool(const int thread_num=0);
    void stop_thread_pool();
    
    virtual bool on_accept(const SocketAddress addr);
    
    void remove_handler(const service_handler *handler);
    
private:
    std::vector<service_handler*> clients_;
    ThpoolServiceLoop thpool_;
};

#endif /* SERVICE_ACCETOR_H */

