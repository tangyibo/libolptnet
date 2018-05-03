#include "net/service_loop_thread.h"
#include "net/service_loop.h"

ServiceLoopThread::ServiceLoopThread()
:loop_(NULL)
,thread_id_(0)
,mutex_locker_()
,cond_var_(mutex_locker_)
{
}

ServiceLoopThread::~ServiceLoopThread()
{
    loop_->quit();
    join_thread(thread_id_);
}

 ServiceLoop * ServiceLoopThread::start_loop()
 {
      thread_id_=create_thread(this,&ServiceLoopThread::thread_routine);
      
      while(NULL==loop_)
          cond_var_.wait();
      
      return loop_;
 }
 
 void ServiceLoopThread::thread_routine()
 {
       ServiceLoop loop;
       
       {
          MutexLockGuard g(mutex_locker_);
          loop_=&loop;
          cond_var_.notify();
      }
      
      loop.run();
 }