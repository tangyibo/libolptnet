#ifndef SERVICE_HANDLER_H
#define SERVICE_HANDLER_H
#include "net/tcp_socket_addr.h"
#include "net/connection_handler.h"

class acceptor_server;
class service_handler : public ConnectionHandler
{
public:
    void set_acceptor(acceptor_server* acceptor)  {     acceptor_ = acceptor;   }

    virtual void on_connect(const SocketAddress &remote);

    virtual int on_read(const input_stream_buffer &input);

    virtual void on_close();

private:
    acceptor_server *acceptor_;
};

#endif /* SERVICE_HANDLER_H */