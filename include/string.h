#ifndef __STRING_H__
#define __STRING_H__

#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif

/* we use this so that we can do without the ctype library */
#define is_digit(c)	((c) >= '0' && (c) <= '9')
#define is_alpha(c)	(((c) >= 'a' && (c) <= 'z') || \
	((c) >= 'A' && (c) <= 'Z'))
#define is_hexa(c)	(((c) >= '0' && (c) <= '9') || \
	((c) >= 'a' && (c) <= 'f') || ((c) >= 'A' && (c) <= 'F'))

void *memset(void *s, int c, size_t n);

void bzero(void *s, size_t n);

int strcmp(const char *s1, const char *s2);

char* strcpy(char *dst, const char *src);

char* strncpy(char *dest, const char *src, size_t n);

int strncmp(const char *s1, const char *s2, size_t n);

size_t strlen(const char *s);

void *memcpy(void *dest, const void *src, size_t n);

char *strdup(const char *s);

const char* strstr(const char* haystack, const char *needle);

#ifdef	__cplusplus
}
#endif

#endif /* __STRING_H__ */
