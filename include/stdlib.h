#ifndef __STDLIB_H__
#define __STDLIB_H__

#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif

#define NULL ((void*)0)

void abort(void);

void *malloc(size_t size);

void *calloc(size_t nmemb, size_t size);

void free(void *ptr);

void *realloc(void *ptr, size_t size);

long int strtol(const char *nptr, char **endptr, int base);

long long int strtoll(const char *nptr, char **endptr, int base);

size_t fwrite(const void *ptr, size_t size, size_t nmemb, void *stream);
int fputc(int c, void *stream);
int fputs(const char *s, void *stream);

extern int stderr;

#ifdef	__cplusplus
}
#endif

#endif /* __STDLIB_H__ */ 
