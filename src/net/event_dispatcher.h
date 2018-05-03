#ifndef EVENT_DISPATCHER_MODEL_H
#define EVENT_DISPATCHER_MODEL_H
#include <vector>

class EventHandler;
typedef std::vector< std::pair<int,int> > ready_event_t;

class EventDispatcher
{
public:

    enum
    {
        EVENT_READ=0,
        EVENT_WRITE=1,
        EVENT_ERROR=2
    };

public:
    
    static EventDispatcher* create_dispatcher(const int flg);

    EventDispatcher ( ) {}
    virtual ~EventDispatcher ( ) {}

    virtual int get_last_errno()=0;
    virtual bool add_event ( const int handle, const bool readable, const bool writeable ) = 0;
    virtual bool mod_event ( const int handle, const bool readable, const bool writeable ) = 0;
    virtual bool del_event ( const int handle ) = 0;
    virtual int handle_events ( ready_event_t& ready, const int timeout_ms = -1 ) = 0;

private:
    EventDispatcher ( const EventDispatcher& other );
    EventDispatcher& operator=(const EventDispatcher& other);
};

#endif /* EVENT_MODEL_H */

