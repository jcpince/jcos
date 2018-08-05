#ifndef __STDINT_H__
#define __STDINT_H__

#include <sched.h>

typedef unsigned int		 uint32_t;
typedef int				     int32_t;
typedef unsigned short	     uint16_t;
typedef short				 int16_t;
typedef unsigned char		 uint8_t;
typedef char				 int8_t;
typedef unsigned char		 bool_t;

#ifndef	__cplusplus
typedef unsigned char		bool;
#endif


#define false	0
#define true	1

#if defined __x86_64__
typedef long int 				int64_t;
typedef unsigned long int 	uint64_t;
typedef long int				intptr_t;
typedef unsigned long int	uintptr_t;
typedef uint64_t				size_t;
typedef int64_t					ssize_t;
typedef uint64_t				ptrdiff_t;
typedef uint64_t				addr_t;
#else
typedef long long int 			int64_t;
typedef unsigned long long int	uint64_t;
typedef int						intptr_t;
typedef unsigned int				uintptr_t;
typedef long unsigned int		size_t;
typedef int32_t						ssize_t;
typedef uint32_t					ptrdiff_t;
typedef uint32_t					addr_t;
#endif

#define KB	(1024UL)
#define MB	(KB*KB)
#define GB	(MB*KB)

#endif /* __STDINT_H__ */
