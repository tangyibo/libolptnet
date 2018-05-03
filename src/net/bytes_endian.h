#ifndef NET_BYTES_ENDIAN_H
#define NET_BYTES_ENDIAN_H
#include <stdint.h>
#include <endian.h>

inline bool is_little_endian ( )
{
    int temp = 1;
    unsigned char byte_order = ( ( unsigned char * ) &temp )[0];
    return (byte_order == 1 );
}

inline uint64_t host_to_network64 ( uint64_t host64 )
{
    return htobe64 ( host64 );
}

inline uint32_t host_to_network32 ( uint32_t host32 )
{
    return htobe32 ( host32 );
}

inline uint16_t host_to_network16 ( uint16_t host16 )
{
    return htobe16 ( host16 );
}

inline uint64_t network_to_host64 ( uint64_t net64 )
{
    return be64toh ( net64 );
}

inline uint32_t network_to_host32 ( uint32_t net32 )
{
    return be32toh ( net32 );
}

inline uint16_t network_to_host16 ( uint16_t net16 )
{
    return be16toh ( net16 );
}

#endif /* BYTES_ENDIAN_H */

