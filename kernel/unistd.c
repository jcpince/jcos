#include <unistd.h>

#define LOOPSPERSECONDS	0x2fffffff

/* sleep - Sleep for the specified number of seconds */
uint64_t sleep(uint64_t seconds)
{
	uint64_t count_down = seconds * LOOPSPERSECONDS;
	
	while (count_down-- > 0);
	
	return 0;
}

/* usleep - suspend execution for microsecond intervals */
uint64_t usleep(uint64_t usec)
{
	uint64_t count_down = (usec * LOOPSPERSECONDS) / 1000000;
	
	while (count_down-- > 0);
	
	return 0;
}
