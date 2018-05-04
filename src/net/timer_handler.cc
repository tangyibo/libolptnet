#include "net/timer_handler.h"
#include "net/service_loop.h"
#include <sys/timerfd.h>  //timerfd_create ,timerfd_settime
#include <assert.h>   //assert
#include <string.h>   //bzero
#include <unistd.h>  //close
#include <errno.h>  //errno
#include <iostream>

TimerHandler::TimerHandler(ServiceLoop *loop, const double interval, const bool repeat)
: timer_fd_(::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC))
, srv_loop_(loop)
, interval_(interval)
, repeat_(repeat)
, started_(false)
{
    assert(timer_fd_ >= 0);
}

TimerHandler::~TimerHandler()
{
    if (timer_fd_ >= 0)
    {
        ::close(timer_fd_);
        timer_fd_ = -1;
    }
}

int TimerHandler::start()
{
    struct itimerspec ptime_internal;
    bzero(&ptime_internal, sizeof (struct itimerspec));
    ptime_internal.it_value.tv_sec = (int) interval_;
    ptime_internal.it_value.tv_nsec = (interval_ - (int) interval_)*1000000000;
    if (repeat_)
    {
        ptime_internal.it_interval.tv_sec = ptime_internal.it_value.tv_sec;
        ptime_internal.it_interval.tv_nsec = ptime_internal.it_value.tv_nsec;
    }

    assert(0 == ::timerfd_settime(timer_fd_, 0, &ptime_internal, NULL));

    int result = srv_loop_->register_event(handle(), true, false, this);
    assert(0 == result);
    
    started_=true;

    return 0;
}

void TimerHandler::stop()
{
    if(!started_)
        return;

    if (repeat_)
    {
        struct itimerspec new_value;
        new_value.it_value.tv_sec = 0;
        new_value.it_value.tv_nsec = 0;
        new_value.it_interval.tv_sec = 0;
        new_value.it_interval.tv_nsec = 0;

        ::timerfd_settime(timer_fd_, 0, NULL, NULL);
        repeat_ = false;
    }

    int result = srv_loop_->delete_event(handle());
    assert(0 == result);
}

void TimerHandler::do_handle_read()
{
    uint64_t howmany;
    ssize_t n = ::read(timer_fd_, &howmany, sizeof (uint64_t));
    assert(n == sizeof (uint64_t));

    on_timer(howmany);

    if (!repeat_)
    {
        stop();
    }
}
