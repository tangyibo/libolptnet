#ifndef MUTEX_LOCK_H
#define MUTEX_LOCK_H
#include <iostream>
#include <cstdio>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>


#if 1
#define CHECK(exp)                                                                                                      \
{                                                                                                                                    \
    int rc = (exp);                                                                                                            \
    if(0!=rc)                                                                                                                     \
    {                                                                                                                                \
         fprintf(stderr, "File:%s, Line:%d Exp:[" #exp "] is true, error:[%d]%s, abort.\n",       \
                   __FILE__, __LINE__, rc,strerror(rc));                                                                \
         abort();                                                                                                                 \
    }                                                                                                                                 \
}
#else
#define CHECK(exp) 
#endif

class MutexLock
{
public:
    MutexLock ( )
    {
        CHECK(pthread_mutex_init(&mutex_, NULL));
    }
    
    ~MutexLock ( )
    {
        CHECK(pthread_mutex_destroy(&mutex_));
    }
    
    void lock ( )
    {
        CHECK(pthread_mutex_lock(&mutex_))
    }
    
    void unlock ( )
    {
        CHECK(pthread_mutex_unlock(&mutex_));
    }

    pthread_mutex_t *getMutexPtr ( )
    {
        return &mutex_;
    }
    
protected:
    MutexLock(const MutexLock &);
    MutexLock& operator=(const MutexLock &);
    
private:
    pthread_mutex_t mutex_; 
};


class MutexLockGuard
{
public:

    MutexLockGuard ( MutexLock &mutex ) : mutex_ ( mutex )
    {
        mutex_.lock ( );
    }

    ~MutexLockGuard ( )
    {
        mutex_.unlock ( );
    }

protected:
    MutexLockGuard ( const MutexLock & );
    MutexLockGuard  operator= ( const MutexLockGuard & );

private:
    MutexLock &mutex_;
};


class ConditionVar
{
    public:
        ConditionVar(MutexLock &mutex)
        :mutex_(mutex)
        {
            CHECK(pthread_cond_init(&cond_, NULL));
        }
        
        ~ConditionVar()
        {
            CHECK(pthread_cond_destroy(&cond_));
        }
        
        void wait()
        {
            CHECK(pthread_cond_wait(&cond_, mutex_.getMutexPtr()));
        }
        
        void notify()
        {
            CHECK(pthread_cond_signal(&cond_));
        }
        
        void notify_all()
        {
            CHECK(pthread_cond_broadcast(&cond_));
        }

protected:
    ConditionVar ( const ConditionVar & );
    ConditionVar operator= ( const ConditionVar & );

private:
    pthread_cond_t cond_;
    MutexLock &mutex_;
};

#endif /* MUTEX_LOCK_H */

