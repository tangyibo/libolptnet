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
#include "acceptor_server.h"

using namespace olptnet;

int main(int argc, char *argv[])
{
    SocketAddress endpoint("127.0.0.1", 6543);
    ServiceLoop loop;
    acceptor_server acceptor(&loop);
    
    acceptor.start_thread_pool(4);
    if (0 == acceptor.open(endpoint))
    {
        loop.run();
    }

    return 0;
}