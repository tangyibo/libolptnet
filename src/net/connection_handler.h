#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H
#include "net/tcp_socket_addr.h"
#include "net/event_handler.h"
#include "net/stream_buffer.h"

class ServiceLoop;
class ConnectionHandler : public EventHandler
{
public:
    ConnectionHandler ( );
    virtual ~ConnectionHandler ( );
    
    void set_service_loop(ServiceLoop *loop);
    virtual int handle ( )  {    return remote_addr_.get_fd();   }

    void open ( SocketAddress remote );
    void close ( );
    
    void send(const char*buf,const size_t len);
    void send(const std::string data);

    virtual void do_handle_read ( );
    virtual void do_handle_write ( );
    virtual void do_handle_error ( );
    
    virtual void on_connect ( const SocketAddress &remote ) =0;
    virtual int on_read ( const input_stream_buffer &input ) =0;
    virtual void on_close ( ) =0;
    
protected:
    void set_remote_addr(const SocketAddress &remote) { remote_addr_=remote; }
    
private:
    SocketAddress remote_addr_;
    ServiceLoop *srv_loop_;

    input_stream_buffer buffer_in_;
    output_stream_buffer buffer_out_;
};

#endif /* TCP_CONNECTION_H */

