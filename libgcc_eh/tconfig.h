#ifndef GCC_TCONFIG_H
#define GCC_TCONFIG_H
#ifndef USED_FOR_TARGET
# define USED_FOR_TARGET
#endif
#include "auto-host.h"
#ifdef IN_GCC
# include "ansidecl.h"
#endif

#define ATTRIBUTE_UNUSED __attribute__ ((__unused__))

#endif /* GCC_TCONFIG_H */
