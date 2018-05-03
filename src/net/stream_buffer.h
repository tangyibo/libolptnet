#ifndef STREAM_BUFFER_H
#define STREAM_BUFFER_H
#include <sstream>
#include <ios>
#include <assert.h>

class stream_buffer
{
public:
    stream_buffer ( );
    stream_buffer ( const stream_buffer &sb );
    virtual ~stream_buffer ( );

    std::string to_string ( )
    {
        return stream_.str ( );
    }

    std::string to_string ( ) const
    {
        return stream_.str ( );
    }

    void clear ( )
    {
        stream_.str ( "" );
    }

    void ignore ( const int bytes )
    {
        std::string tmp = stream_.str ( );
        assert(bytes>=0 && bytes<=(int)tmp.size ());

        stream_.str ( tmp.substr ( bytes ) );       
    }

    const size_t length ( )
    {
        return stream_.str ( ).size ( );
    }

    const size_t length ( ) const
    {
        return stream_.str ( ).size ( );
    }

protected:
    std::stringstream stream_;
};

class input_stream_buffer : public stream_buffer
{
public:
    input_stream_buffer ( );
    input_stream_buffer ( const stream_buffer &sb );
    virtual ~input_stream_buffer ( );

    int read ( const int fd, int& sava_errno );
};

class output_stream_buffer : public stream_buffer
{
public:
    output_stream_buffer ( );
    output_stream_buffer ( const stream_buffer &sb );
    virtual ~output_stream_buffer ( );

    inline bool empty ( )
    {
        return stream_.str ( ).empty ( );
    }

    void put ( const char *ptr, size_t len );
    void put ( const std::string &msg );
};

#endif /* STREAM_BUFFER_H */

