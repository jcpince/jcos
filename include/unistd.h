#ifndef __UNISTD_H__
#define __UNISTD_H__

#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif

/* sleep - Sleep for the specified number of seconds */
uint64_t sleep(uint64_t seconds);

/* usleep - suspend execution for microsecond intervals */
uint64_t usleep(uint64_t usec);

#ifdef	__cplusplus
}
#endif

#endif /* __UNISTD_H__ */
