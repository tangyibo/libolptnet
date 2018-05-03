#include "net/event_dispatcher.h"
#include "net/dispatcher/select.cc"
#include "net/dispatcher/poll.cc"
#include "net/dispatcher/epoll.cc"

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