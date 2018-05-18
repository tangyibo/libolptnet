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
#include "base/posix_thread.h"

using namespace olptnet;

SocketAddress endpoint("127.0.0.1", 6543);
ServiceLoop loop;
acceptor_server acceptor(&loop);

void *server(void *arg)
{
     loop.run();
     
     return NULL;
}

int main( int argc, char *argv[] )
{
    acceptor.start_thread_pool(4);
    if ( 0 == acceptor.open( endpoint ) )
    {
       pthread_t thread=create_thread(server,NULL);
       sleep(50);
       acceptor.stop_thread_pool();
       loop.quit();
       join_thread(thread);
    }

    return 0;
}