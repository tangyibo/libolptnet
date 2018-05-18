// Copyright tang.  All rights reserved.
// https://github.com/tangyibo/libolptnet
//
// Use of this source code is governed by a BSD-style license
//
// Author: tang (inrgihc@126.com)
// Data : 2018/5/2
// Location: beijing , china
/////////////////////////////////////////////////////////////
#include "net/event_dispatcher.h"
#include "net/dispatcher/select.cc"
#include "net/dispatcher/poll.cc"
#include "net/dispatcher/epoll.cc"

namespace olptnet{

EventDispatcher* EventDispatcher::create_dispatcher(const int flg)
{
    switch(flg)
    {
        case 0:
            return new DispatcherSelect();
        case 1:
            return new DispatcherPoll();
        case 2:
            return new DispatcherEpoll();
        default:
            break;
    }
    
    return NULL;
}

} //end namespace olptnet