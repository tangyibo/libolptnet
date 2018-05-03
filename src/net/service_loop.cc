#include "net/service_loop.h"
#include "net/event_handler.h"
#include "net/event_dispatcher.h"
#include <sys/eventfd.h>
#include <assert.h>
#include <stdint.h>

class WakeupHandler : public EventHandler
{
    friend class ServiceLoop;
    
public:
    WakeupHandler(ServiceLoop *loop)
    :srv_loop_(loop)
    ,ev_fd_(::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC))
    {
        assert(ev_fd_!=-1);
    }

    ~WakeupHandler()
    {
        close();
    }

    void close()
    {
        if (ev_fd_>-1)
        {
            int result = srv_loop_->delete_event(ev_fd_);
            assert(0 == result);

            ::close(ev_fd_);
            ev_fd_ = -1;
        }
    }

    void open()
    {
        int result = srv_loop_->register_event(handle(), true, false, this);
        assert(0 == result);
    }
    
    virtual int handle ( )
    {
        return ev_fd_;
    }

    virtual void do_handle_read()
    {
        uint64_t one;
        ssize_t n = ::read(handle ( ), &one, sizeof(uint64_t));
        assert (n == sizeof(uint64_t));
        assert(1==one);
    }

    virtual void do_handle_write()
    {
    }

    virtual void do_handle_error()
    {
    }

    void write_some()
    {
        uint64_t one = 1;
        ssize_t n = ::write(handle ( ), &one, sizeof (uint64_t));
        assert(n == sizeof(uint64_t));
    }

private:
    ServiceLoop *srv_loop_;
    int ev_fd_;
};

////////////////////////////////////////////////////////
ServiceLoop::ServiceLoop()
:quit_(false)
,dispatcher_(NULL)
,handlers_()
,functors_()
,wakeup_(new WakeupHandler(this))
{
    dispatcher_ = EventDispatcher::create_dispatcher(2);

    assert(NULL != dispatcher_);
    assert(NULL!=wakeup_);
    
    handlers_.clear();
    
    wakeup_->open();
}

ServiceLoop::~ServiceLoop()
{
    if (dispatcher_)
    {
        delete dispatcher_;
        dispatcher_ = NULL;
    }

    handlers_.clear();
    
    if(wakeup_)
    {
        delete wakeup_;
        wakeup_=NULL;
    }
}

int ServiceLoop::register_event(const int h, bool readable, bool writeable, EventHandler *handler)
{
    assert(h > 0);
    assert(NULL != handler);
    if (!dispatcher_->add_event(h, readable, writeable))
    {
        return dispatcher_->get_last_errno();
    }

    handlers_[h] = handler;
    return 0;
}

int ServiceLoop::modify_event(const int h, bool readable, bool writeable, EventHandler *handler)
{
    assert(h > 0);
    assert(NULL != handler);
    if (!dispatcher_->mod_event(h, readable, writeable))
    {
        return dispatcher_->get_last_errno();
    }

    if (handler != handlers_[h])
    {
        handlers_[h] = handler;
    }

    return 0;
}

int ServiceLoop::delete_event(const int h)
{
    assert(h > 0);
    handlers_list_t::iterator it = handlers_.find(h);
    if (it != handlers_.end())
    {
        if (!dispatcher_->del_event(h))
        {
            return dispatcher_->get_last_errno();
        }

        handlers_.erase(it);
    }

    return 0;
}

void ServiceLoop::add_run(RunableFunctor &f)
{
    functors_.push(f);
}

void ServiceLoop::wakeup_event()
{
    if(wakeup_)
    {
        wakeup_->write_some();
    }
}

#include "base/posix_thread.h"
void ServiceLoop::run()
{
    std::cout << "thread: ServiceLoop::run()--" << (int) gettid() << std::endl;

    int timeout_ms = 2;
    ready_event_t ready;
    while(!quit_)
    {
        ready.clear();
        int ret = dispatcher_->handle_events(ready, timeout_ms);
        switch (ret)
        {
            case -1:
                timeout_ms = 2;
                break;
            case 0:
                timeout_ms = 2;
                break;
            default:
                timeout_ms = 1;
                break;
        }

        ready_event_t::iterator it;
        for (it = ready.begin(); it != ready.end(); ++it)
        {
            int fd = it->first;
            int ev = it->second;
            EventHandler *handler = handlers_[fd];

            if (ev == EventDispatcher::EVENT_READ && handler)
                handler->do_handle_read();

            if (ev == EventDispatcher::EVENT_WRITE && handler)
                handler->do_handle_write();

            if (ev == EventDispatcher::EVENT_ERROR && handler)
                handler->do_handle_error();
        }

        RunableFunctor fun;
        while(functors_.pop(fun))
            fun();
        
    }//while
}

void ServiceLoop::quit()
{
    quit_=true;
    
    wakeup_event();
}