#ifndef TIMER_HANDLER_H
#define TIMER_HANDLER_H
#include "net/event_handler.h"
#include <stdint.h>

class ServiceLoop;
class TimerHandler: public EventHandler
{
public:
    TimerHandler (ServiceLoop *loop,const double interval=0.,const bool repeat=false);
    virtual ~TimerHandler ( );
    
    int start();
    void stop();

    void set_interval(const double interval) { interval_=interval; }
    const double get_interval() { return interval_; }
    void set_repeat(const bool repeat) { repeat_=repeat; }
    const bool is_repeat() { return repeat_; }

    virtual int handle ( )  {  return timer_fd_; }
    
    virtual void do_handle_read ( );
    virtual void do_handle_write ( ) {}
    virtual void do_handle_error ( ) {}
    virtual void do_handle_timeout ( ) {}
    
    virtual void on_timer(uint64_t expire)=0;

private:
    int timer_fd_;
    ServiceLoop *srv_loop_;
    volatile double interval_;
    volatile bool repeat_;
    volatile bool started_;
};

#endif /* TIMER_HANDLER_H */

