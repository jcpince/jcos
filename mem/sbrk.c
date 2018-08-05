#include <stdlib.h>

#define MALLOC_BUFFER_SIZE	4*MB
uint8_t malloc_buffer[MALLOC_BUFFER_SIZE];

void* sbrk(ptrdiff_t size)
{
    static uint8_t *current = &malloc_buffer[0];
    static uint8_t *max = &malloc_buffer[MALLOC_BUFFER_SIZE];
    uint8_t *old = current;
    if ((current + size) < max)
        current += size;
    else
        current = max;
    return old;
}
