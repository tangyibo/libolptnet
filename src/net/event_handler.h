// Copyright tang.  All rights reserved.
// https://github.com/tangyibo/libolptnet
//
// Use of this source code is governed by a BSD-style license
//
// Author: tang (inrgihc@126.com)
// Data : 2018/5/2
// Location: beijing , china
/////////////////////////////////////////////////////////////
#ifndef _EVENT_HANDLER_H_
#define _EVENT_HANDLER_H_

namespace olptnet{
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

} //end namespace olptnet

#endif /* EVENT_HANDLER_H */

