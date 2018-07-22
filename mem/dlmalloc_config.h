#ifndef __DLMALLOC_CONFIG__
#define __DLMALLOC_CONFIG__

#include <stdlib.h>
#include <stdio.h>

#define ENOMEM      (12)
#define EINVAL      (22)

//#define LACKS_UNISTD_H      1
#define LACKS_SYS_PARAM_H   1
#define LACKS_SYS_MMAN_H    1
#define LACKS_STRINGS_H     1
#define LACKS_TIME_H        1

#define USE_BUILTIN_FFS     1
#define NO_MALLOC_STATS     1
#define HAVE_MMAP           0

// LACKS_UNISTD_H, LACKS_FCNTL_H, LACKS_SYS_PARAM_H, LACKS_SYS_MMAN_H
// LACKS_STRINGS_H, LACKS_STRING_H, LACKS_SYS_TYPES_H,  LACKS_ERRNO_H
// LACKS_STDLIB_H LACKS_SCHED_H LACKS_TIME_H

#endif /* __DLMALLOC_CONFIG__ */
