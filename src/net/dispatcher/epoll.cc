#include "net/event_dispatcher.h"
#include <sys/epoll.h> //epoll_create,epoll_ctrl,epoll_wait
#include <assert.h> //assert
#include <errno.h> //errno
#include <unistd.h>
#include <vector> //std::vector

/*
 * I/O复用之EPOLL模型调度分发
 */
class DispatcherEpoll : public EventDispatcher
{
public:
    DispatcherEpoll();
    virtual ~DispatcherEpoll();

    virtual int get_last_errno()
    {
        return last_errno_;
    }

    virtual bool add_event(const int handle, const bool readable, const bool writeable);
    virtual bool mod_event(const int handle, const bool readable, const bool writeable);
    virtual bool del_event(const int handle);
    virtual int handle_events(ready_event_t& ready, const int timeout_ms = -1);

private:
    int epoll_fd_;
    int last_errno_;
    int fd_count_;
};

typedef struct epoll_event epoll_event_t;

DispatcherEpoll::DispatcherEpoll()
: epoll_fd_(-1)
, last_errno_(0)
, fd_count_(0)
{
    epoll_fd_ = ::epoll_create(1024);
    assert(epoll_fd_ > 0);
}

DispatcherEpoll::~DispatcherEpoll()
{
    if (epoll_fd_ > 0)
    {
        ::close(epoll_fd_);
        epoll_fd_ = -1;
        fd_count_ = 0;
    }
}

bool DispatcherEpoll::add_event(const int handle, const bool readable, const bool writeable)
{
    epoll_event_t ev;
    ev.data.fd = handle;

    if (readable)
        ev.events = EPOLLIN;

    if (writeable)
        ev.events = EPOLLOUT;

    //ev.events|=EPOLLET;
    if (::epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, handle, &ev) == -1)
    {
        last_errno_ = errno;
        return false;
    }

    fd_count_++;

    return true;
}

bool DispatcherEpoll::mod_event(const int handle, const bool readable, const bool writeable)
{
    epoll_event_t ev;
    ev.data.fd = handle;

    if (readable)
        ev.events = EPOLLIN;

    if (writeable)
        ev.events = EPOLLOUT;

    //ev.events|=EPOLLET;
    if (::epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, handle, &ev) == -1)
    {
        last_errno_ = errno;
        return false;
    }

    return true;
}

bool DispatcherEpoll::del_event(const int handle)
{
    epoll_event_t ev;

    if (::epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, handle, &ev) == -1)
    {
        if (ENOENT != errno)
        {
            last_errno_ = errno;
            return false;
        }
    }

    fd_count_++;

    return true;
}

int DispatcherEpoll::handle_events(ready_event_t& ready, const int timeout_ms)
{
    std::vector<epoll_event_t> look_events;
    look_events.resize(fd_count_);

    int nfds = ::epoll_wait(epoll_fd_, &look_events[0], fd_count_, timeout_ms);
    if (nfds < 0)
    {
        last_errno_ = errno;
        return -1;
    }
    else if (0 == nfds)
    {
        return 0;
    }
    else
    {
        for (int i = 0; i < nfds; ++i)
        {
            epoll_event_t &ev = look_events[i];

            if (ev.events & (EPOLLIN | EPOLLPRI))
            {
                ready.push_back( std::make_pair<int,int>(ev.data.fd,EventDispatcher::EVENT_READ) );
            }

            if (ev.events & EPOLLOUT)
            {
                ready.push_back( std::make_pair<int,int>(ev.data.fd,EventDispatcher::EVENT_WRITE) );
            }

            if (ev.events & (EPOLLHUP | EPOLLERR))
            {
                ready.push_back( std::make_pair<int,int>(ev.data.fd,EventDispatcher::EVENT_ERROR) );
            }

        }

    }

    return nfds;
}