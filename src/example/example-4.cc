#include "net/timer_handler.h"
#include "net/service_loop.h"
#include <iostream>
#include <time.h>

class my_timer : public TimerHandler
{
public:
    my_timer(ServiceLoop *loop,int interval,const bool repeat=false)
	:TimerHandler(loop,interval,repeat)
    {
    }
    
    virtual void on_timer(uint64_t expire)
    {
           std::cout<<"["<<time(NULL)<<"]expire times:"<<expire<<std::endl;
    }
};

int main(int argc, char *argv[])
{
    ServiceLoop loop;
    my_timer timer(&loop,2,true);
    
    if (0 == timer.start())
    {
        loop.run();
    }

    return 0;
}
