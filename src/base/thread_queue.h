// Copyright tang.  All rights reserved.
// https://github.com/tangyibo/libolptnet
//
// Use of this source code is governed by a BSD-style license
//
// Author: tang (inrgihc@126.com)
// Data : 2018/5/2
// Location: beijing , china
/////////////////////////////////////////////////////////////
#ifndef __THREAD_QUEUE_HEADER_H__
#define __THREAD_QUEUE_HEADER_H__
#include <deque> 
#include "base/mutex_lock.h"

namespace olptnet{

template <typename T>
class ThreadQueue
{
public:
    ThreadQueue ( )
    :tasks_list_()
    ,tasks_mutex_()
    {
    }

    ~ThreadQueue ( )
    {
    }

    void push ( const T& t )
    {
        MutexLockGuard lock ( tasks_mutex_ );
        tasks_list_.push_back ( t );
    }

    bool pop( T &t )
    {
        MutexLockGuard lock ( tasks_mutex_ );
        if ( !tasks_list_.empty ( ) )
        {
            t = tasks_list_.front ( );
            tasks_list_.pop_front ( );
           
            return true;
        }
        
        return false;
    }

    const int size ( )
    {
        MutexLockGuard lock ( tasks_mutex_ );
        return tasks_list_.size ( );
    }

private:
    std::deque<T> tasks_list_;
    MutexLock tasks_mutex_;
};  

}//end namespace olptnet

#endif
