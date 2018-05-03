#include "service_handler.h"
#include "acceptor_server.h"
#include <iostream>

void service_handler::on_connect(const SocketAddress &remote)
{
    std::cout<<"Connect:"<<remote.to_string().c_str()<<std::endl;
}

int service_handler::on_read(const input_stream_buffer &input)
{
    std::string msg=input.to_string();
    std::cout<< "Recv:"<<msg.c_str()<<std::endl;
    
    msg+="\n";
    send(msg);

    return input.length();
}

void service_handler::on_close()
{
    acceptor_->remove_handler(this);
    std::cout<<"Close...:"<<std::endl;
}
