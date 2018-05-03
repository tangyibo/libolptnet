#include "net/event_dispatcher.h"
#include "net/event_handler.h"
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>

/*
 * I/O复用之SELECT模型调度分发
 */
class DispatcherSelect : public EventDispatcher
{
public:
    DispatcherSelect();
    virtual ~DispatcherSelect();

    virtual int get_last_errno()
    {
        return last_errno_;
    }

    virtual bool add_event(const int handle, const bool readable, const bool writeable);
    virtual bool mod_event(const int handle, const bool readable, const bool writeable);
    virtual bool del_event(const int handle);
    virtual int handle_events(ready_event_t& ready, const int timeout_ms = -1);

private:
    int maxfd_;
    int last_errno_;
    fd_set rsets_;
    fd_set wsets_;
    fd_set esets_;
};

DispatcherSelect::DispatcherSelect()
: maxfd_(-1)
, last_errno_(0)
{
    FD_ZERO(&rsets_);
    FD_ZERO(&wsets_);
    FD_ZERO(&esets_);
}

DispatcherSelect::~DispatcherSelect()
{

}

bool DispatcherSelect::add_event(const int handle, const bool readable, const bool writeable)
{
    if (readable)
    {
        FD_SET(handle, &rsets_);
    }

    if (writeable)
    {
        FD_SET(handle, &wsets_);
    }

    FD_SET(handle, &esets_);

    if (handle > maxfd_)
    {
        maxfd_ = handle;
    }

    return true;
}

bool DispatcherSelect::mod_event(const int handle, const bool readable, const bool writeable)
{
    if (readable)
    {
        FD_SET(handle, &rsets_);
    }
    else
    {
        FD_CLR(handle, &rsets_);
    }

    if (writeable)
    {
        FD_SET(handle, &wsets_);
    }
    else
    {
        FD_CLR(handle, &wsets_);
    }

    return true;
}

bool DispatcherSelect::del_event(const int handle)
{
    FD_CLR(handle, &rsets_);
    FD_CLR(handle, &wsets_);
    FD_CLR(handle, &esets_);

    return true;
}

int DispatcherSelect::handle_events(ready_event_t& ready, const int timeout_ms)
{
    int ret = 0;
    struct timeval tv;
    fd_set readset = rsets_;
    fd_set writeset = wsets_;
    fd_set exceptset = esets_;

    if (timeout_ms >= 0)
    {
        tv.tv_sec = timeout_ms / 1000;
        tv.tv_usec = (timeout_ms % 1000) * 1000;
        ret = ::select(maxfd_ + 1, &readset, &writeset, &exceptset, &tv);
    }
    else
    {
        ret = ::select(maxfd_ + 1, &readset, &writeset, &exceptset, NULL);
    }

    if (ret < 0)
    {
        last_errno_ = errno;
        return -1;
    }
    else if (0 == ret)
    {
        return 0;
    }
    else
    {
        for (int i = 0; i < maxfd_ + 1; ++i)
        {
            if (FD_ISSET(i, &readset))
            {
                ready.push_back(std::make_pair<int,int>(i,EventDispatcher::EVENT_READ) );
            }

            if (FD_ISSET(i, &writeset))
            {
                ready.push_back( std::make_pair<int,int>(i,EventDispatcher::EVENT_WRITE ) );
            }

            if (FD_ISSET(i, &exceptset))
            {
               ready.push_back( std::make_pair<int,int>(i,EventDispatcher::EVENT_ERROR) );
            }

        }
    }

    return ret;
}
