#ifndef __STDIO_H__
#define __STDIO_H__

#include <stdlib.h>
#include <stdarg.h>

#ifdef	__cplusplus
extern "C" {
#endif

#define CFG_PBSIZE	256
#define stderr		2

/*#define printf(...)	{}
#define fprintf(...)	{}*/

void printk (const char *fmt, ...);

int sprintf(char *str, const char *format, ...);

int snprintf(char *str, size_t size, const char *format, ...);

int vsprintf(char *buf, const char *fmt, va_list args);

static inline char *hex8(char *buffer, uint8_t val) {sprintf(buffer, "%x", val); return buffer;}
static inline char *hex16(char *buffer, uint16_t val) {sprintf(buffer, "%x", val); return buffer;}
static inline char *hex32(char *buffer, uint32_t val) {sprintf(buffer, "%x", val); return buffer;}
static inline char *hex64(char *buffer, uint64_t val) {sprintf(buffer, "%lx", val); return buffer;}

#ifdef	__cplusplus
}
#endif

#endif /* __STDIO_H__ */ 
