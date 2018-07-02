#ifndef __DLMALLOC_CONFIG__
#define __DLMALLOC_CONFIG__

#include <stdlib.h>
#include <stdio.h>

#define ENOMEM      (-5)

#define LACKS_UNISTD_H
#define LACKS_SYS_PARAM_H
#define LACKS_SYS_MMAN_H

/* Use the supplied emulation of sbrk */
#define MORECORE            sbrk
#define MORECORE_CONTIGUOUS 1
#define MORECORE_FAILURE    ((void*)(-1))

#define HAVE_MMAP 0

#undef USE_MALLOC_LOCK
#undef USE_DL_PREFIX

void* sbrk(ptrdiff_t size)
{
    printk("Calling %s(0x%lx)\n", size);
    return NULL;
}

#define fprintf(x, fmt, args...) printk(fmt, ##args)

#endif /* __DLMALLOC_CONFIG__ */
