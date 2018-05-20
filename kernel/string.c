#include <string.h>
#include <stdint.h>
#include <stdlib.h>

void *memset(void *s, int c, size_t n)
{
	char *ptr = (char*)s;
	
	if (!n || !s) return s;
	
	do {
		ptr[n-1] = (unsigned char)(c&0xff);
	} while(n-- > 0);
	
	return s;
}

void bzero(void *s, size_t n)
{
	memset(s, 0, n);
}

int strcmp(const char *s1, const char *s2)
{
	while(*s1 && (*s1 == *s2))
	{
		s1++;s2++;
	}
	
	return *s2 - *s1;
}

char* strcpy(char *dst, const char *src)
{
	char *result = dst;
	while(*src)
	{
		*dst++ = *src++;
	}
	/* copy the \0 character at the end */
	*dst = *src;
	
	return result;
}

char* strncpy(char *dest, const char *src, size_t n)
{
	size_t i;

	for (i = 0 ; i < n && src[i] != '\0' ; i++)
		dest[i] = src[i];
	for ( ; i < n ; i++)
		dest[i] = '\0';

	return dest;
}


int strncmp(const char *s1, const char *s2, size_t n)
{
	if (!n) return 0;
	
	while(*s1 && (*s1 == *s2) && (n > 0))
	{
		s1++;s2++;n--;
	}
	
	return *s2 - *s1;
}

size_t strlen(const char *s)
{
	size_t size = 0;
	while(*s++) {
		size++;
	}
	
	return size;
}

void *memcpy(void *dest, const void *src, size_t n)
{
	char *d = (char*)dest, *s = (char*)src;
	
	if (!n) return dest;
	
	do {
		*d++ = *s++;
	} while(--n > 0);
	
	return dest;
}

char *strdup(const char *s)
{
	char *newstr = (char*)malloc(strlen(s));
	strcpy(newstr, s);
	return newstr;
}

char* strstr(const char* haystack, const char *needle)
{
	int len = strlen(needle);
	
	if (!len) return (char *)haystack;

	while (*haystack)
	{
		if (!strncmp(haystack, needle, len))
			return (char *)haystack;
		haystack++;
	}
	return (char *)NULL;
}

