// Copyright tang.  All rights reserved.
// https://github.com/tangyibo/libolptnet
//
// Use of this source code is governed by a BSD-style license
//
// Author: tang (inrgihc@126.com)
// Data : 2018/5/2
// Location: beijing , china
/////////////////////////////////////////////////////////////
#ifndef SERVICE_LOOP_THREAD_H
#define SERVICE_LOOP_THREAD_H
#include "base/mutex_lock.h"
#include "base/posix_thread.h"

namespace olptnet{

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

} //end namespace olptnet

#endif /* SERVICE_LOOP_THREAD_H */

