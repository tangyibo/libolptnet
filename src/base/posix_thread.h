#ifndef MEMBER_THREAD_H
#define MEMBER_THREAD_H
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>

#define gettid()  static_cast<pid_t>(::syscall(SYS_gettid))

template<class T>
class ThreadHelper
{  
public:
    static pthread_t thread_create( T *obj, void (T::*mf)() )
    {
        ThreadHelper<T> *p=new ThreadHelper<T>(obj,mf);
        pthread_t thread_id;
        
        int rc=pthread_create(&thread_id,NULL,&(ThreadHelper<T>::thread_function),(void *)p);
        if(0!=rc)
        {
            fprintf(stderr,"pthread_create() error: %s, at %s:%d\n",strerror(rc),__FILE__,__LINE__);
            exit(1);
        }
        return thread_id;
    }
    
    ThreadHelper( T *obj, void  (T::*mf)())
    :obj_(obj),mf_(mf)
    {}
    
protected:
    
    static void * thread_function(void *param)
    {
        ThreadHelper<T> *p=(ThreadHelper<T> *)param;
        T* obj=p->obj_;
        void  (T::*mf)()=p->mf_;
        
        delete p;
        ( obj->*mf )();

        return 0;
    }
    
    T *obj_;
    void  (T::*mf_)();
};

template<class T>
static pthread_t create_thread ( T *obj, void (T::*mf )( ) )
{
    return ThreadHelper<T>::thread_create ( obj, mf );
}

inline pthread_t create_thread ( void* (*func )( void * ), void *arg )
{
    pthread_t thread_id;

    int rc = pthread_create ( &thread_id, NULL, func, ( void * ) arg );
    if ( 0 != rc )
    {
        fprintf ( stderr, "pthread_create() error: %s, at %s:%d\n", strerror ( rc ), __FILE__, __LINE__ );
        exit ( 1 );
    }
    return thread_id;
}

inline void join_thread(const pthread_t pid)
{
    pthread_join(pid,NULL);
}

#endif /* MEMBER_THREAD_H */

