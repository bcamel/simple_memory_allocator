#if !defined(MEMHDR_H)
#define MEMHDR_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef char ALIGN[16];

union header
{
    struct
    {
        size_t size;
        bool is_free;
        union header *next;
    } s;

    ALIGN stub;
};
typedef union header header_t;

#endif