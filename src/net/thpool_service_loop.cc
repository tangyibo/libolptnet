#include "thpool_service_loop.h"
#include "net/service_loop.h"
#include "net/service_loop_thread.h"
#include <iostream>
#include <time.h>

ThpoolServiceLoop::ThpoolServiceLoop(ServiceLoop *baseloop)
:thread_num_(0)
,base_loop_(baseloop)
,loop_threads_()
,ev_loops_()
{
}

ThpoolServiceLoop::~ThpoolServiceLoop()
{
    close();
}

void ThpoolServiceLoop::open()
{
    for(int i=0;i<thread_num_;++i)
    {
        ServiceLoopThread *obj=new ServiceLoopThread();
        ev_loops_.push_back(obj->start_loop());
        loop_threads_.push_back(obj);
    }
}

void ThpoolServiceLoop::close()
{
     std::vector<ServiceLoopThread*>::iterator it;
     for(it=loop_threads_.begin();it!=loop_threads_.end();++it)
         delete *it;
     
     loop_threads_.clear();
     ev_loops_.clear();
}

 ServiceLoop * ThpoolServiceLoop::get_loop()
 {
     //该函数所在的线程==base_loop_所在的线程
     if(ev_loops_.empty())
         return base_loop_;
     
     static int index=0;
     
     size_t loop_count=ev_loops_.size();
          
     return ev_loops_[index++%loop_count];
 }