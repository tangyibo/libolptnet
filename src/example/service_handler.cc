// Copyright tang.  All rights reserved.
// https://github.com/tangyibo/libolptnet
//
// Use of this source code is governed by a BSD-style license
//
// Author: tang (inrgihc@126.com)
// Data : 2018/5/2
// Location: beijing , china
/////////////////////////////////////////////////////////////
#include "service_handler.h"
#include "acceptor_server.h"
#include <iostream>
#include "base/posix_thread.h"

void service_handler::on_connect(const SocketAddress &remote)
{
    std::cout<<"Connect:"<<remote.to_string().c_str()<<std::endl;
}


int service_handler::on_read(const input_stream_buffer &input)
{
    std::string msg=input.to_string();
    std::cout<<"["<< (int) gettid() << "]Recv:"<<msg.c_str()<<std::endl;
    
    msg+="\n";
    send(msg);

    return input.length();
}

void service_handler::on_close()
{
    acceptor_->remove_handler(this);
    std::cout<<"Close...:"<<std::endl;
}
