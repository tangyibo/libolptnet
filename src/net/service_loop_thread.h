#ifndef SERVICE_LOOP_THREAD_H
#define SERVICE_LOOP_THREAD_H
#include "base/mutex_lock.h"
#include "base/posix_thread.h"

class ServiceLoop;
class ServiceLoopThread
{
public:
    ServiceLoopThread ( );
    virtual ~ServiceLoopThread ( );

    ServiceLoop * start_loop();
    
    void thread_routine();
    
protected:
    ServiceLoopThread ( const ServiceLoopThread& other );
    ServiceLoopThread& operator=(const ServiceLoopThread& other);
    
private:
    ServiceLoop *loop_;
    pthread_t thread_id_;
    MutexLock mutex_locker_;
    ConditionVar cond_var_;
};

#endif /* SERVICE_LOOP_THREAD_H */

