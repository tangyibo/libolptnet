#include "stream_buffer.h"
#include <sys/types.h>   
#include <sys/socket.h>
#include <errno.h>

stream_buffer::stream_buffer( )
{
}

stream_buffer::stream_buffer (const stream_buffer &sb)
{
    stream_.str("");
    
    std::string str=sb.stream_.str();
    stream_.write(str.c_str(),str.size());
}

stream_buffer::~stream_buffer( )
{
        clear();
}

//////////////////////////////////////////////////////////////////
input_stream_buffer::input_stream_buffer ()
:stream_buffer( )
{
    
}
 input_stream_buffer::input_stream_buffer (const stream_buffer &sb)
 :stream_buffer(sb)
 {
     
 }
 
input_stream_buffer::~ input_stream_buffer ()
{

}


int input_stream_buffer::read(const int fd,int& sava_errno)
{
    int read_bytes=0;
    const int len = 64 * 1024;
    char buffer[len];
    int ret=0;
        
    do
    {
        ret = ::recv(fd, buffer, len, 0);
        if (ret < 0)
        {
            sava_errno = errno;
            return ret;
        }
        else if(0==ret)
        {
            break;
        }
        else if (ret > 0)
        {
            stream_.write(buffer, ret);
            read_bytes+=ret;
            
            if(ret<len)
                break;
        }
    }while (ret > 0);

    return read_bytes;
}

/////////////////////////////////////////////////////////
output_stream_buffer::output_stream_buffer ()
:stream_buffer( )
{
    
}
 output_stream_buffer::output_stream_buffer (const stream_buffer &sb)
 :stream_buffer(sb)
 {
     
 }
 
output_stream_buffer::~ output_stream_buffer ()
{
    
}

void output_stream_buffer::put(const char *ptr,size_t len)
 {
     stream_.write(ptr,len);
 }
 
 void output_stream_buffer::put(const std::string &msg)
{
    stream_.write(msg.c_str(),msg.size());
}