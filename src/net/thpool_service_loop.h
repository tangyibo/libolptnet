#ifndef THPOOL_SERVICE_LOOP_H
#define THPOOL_SERVICE_LOOP_H
#include <vector>

class ServiceLoopThread;
class ServiceLoop;
class ThpoolServiceLoop
{
public:
    ThpoolServiceLoop (ServiceLoop *baseloop);
    virtual ~ThpoolServiceLoop ( );
    
    inline void set_thread_num(const int n)
    {
        if(n>0)
        {
            thread_num_=n;
        }
    }
            
    void open();
    void close();
    
    ServiceLoop * get_loop();
    
protected:
    ThpoolServiceLoop ( const ThpoolServiceLoop& other );
    ThpoolServiceLoop& operator=(const ThpoolServiceLoop& other);
    
private:
    int thread_num_;
    ServiceLoop* base_loop_;
    std::vector<ServiceLoopThread*> loop_threads_;
    std::vector<ServiceLoop *> ev_loops_;
};

#endif /* THPOOL_SERVICE_LOOP_H */

