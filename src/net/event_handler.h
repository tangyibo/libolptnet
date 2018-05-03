#ifndef _EVENT_HANDLER_H_
#define _EVENT_HANDLER_H_

/*
 *事件处理器
 */
class EventHandler
{
public:

    EventHandler ( ){}
    virtual ~EventHandler ( ) {}

    virtual int handle ( )=0;
    
    virtual void do_handle_read ( ) = 0;
    virtual void do_handle_write ( ) = 0;
    virtual void do_handle_error ( ) = 0;
    virtual void do_handle_timeout() {}
    
private:
    EventHandler ( const EventHandler& other );
    EventHandler& operator=(const EventHandler& other);
};


#endif /* EVENT_HANDLER_H */

