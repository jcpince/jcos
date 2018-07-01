#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#if defined __x86_64__
#define LONG_MAX	9223372036854775807L
#define LONG_MIN	(-LONG_MAX-1)
#else
#define LONG_MAX	2147483647L
#define LONG_MIN	(-LONG_MAX-1)
#endif
/* TODO: Implement those functions */

int errno = 0;

#define MALLOC_BUFFER_SIZE	4*MB
static char malloc_buffer[MALLOC_BUFFER_SIZE];

void abort(void)
{
	while(1);
}

void *malloc(size_t size)
{
	static addr_t freeMemoryBase = (addr_t)malloc_buffer;
	static size_t bytesfree = MALLOC_BUFFER_SIZE;
	size = (size + 7) / 8 * 8;
	if (bytesfree < size)
		return NULL;
	freeMemoryBase += size;
	bytesfree -= size;
	return (void*)(freeMemoryBase - size);
}

void *calloc(size_t nmemb, size_t size)
{
	void *mem = NULL;

	if (!nmemb || !size) return NULL;

	size = (size*nmemb + 7) / 8 * 8;
	mem = malloc(size);

	if (!mem) return NULL;

	return memset(mem, 0, size);
}

void free(void *ptr)
{
	if (ptr) return;
	ptr = NULL; /* Does nothing yet */

	return;
}

void *realloc(void *ptr, size_t size)
{
	void *newptr = malloc(size);

	memcpy(newptr, ptr, size);
	free(ptr);
	return newptr;
}

/*
 * Convert a string to a long integer.
 *
 * Ignores `locale' stuff.  Assumes that the upper and lower case
 * alphabets and digits are each contiguous.
 */
long int strtol(const char *nptr, const char **endptr, int base)
{
	register const unsigned char *s = (const unsigned char *)nptr;
	register unsigned long acc;
	register int c;
	register unsigned long cutoff;
	register int neg = 0, any, cutlim;

	/*
	 * Skip white space and pick up leading +/- sign if any.
	 * If base is 0, allow 0x for hex and 0 for octal, else
	 * assume decimal; if base is already 16, allow 0x.
	 */
	do {
		c = *s++;
	} while (isspace(c));
	if (c == '-') {
		neg = 1;
		c = *s++;
	} else if (c == '+')
		c = *s++;
	if ((base == 0 || base == 16) &&
	    c == '0' && (*s == 'x' || *s == 'X')) {
		c = s[1];
		s += 2;
		base = 16;
	}
	if (base == 0)
		base = c == '0' ? 8 : 10;

	/*
	 * Compute the cutoff value between legal numbers and illegal
	 * numbers.  That is the largest legal value, divided by the
	 * base.  An input number that is greater than this value, if
	 * followed by a legal input character, is too big.  One that
	 * is equal to this value may be valid or not; the limit
	 * between valid and invalid numbers is then based on the last
	 * digit.  For instance, if the range for longs is
	 * [-2147483648..2147483647] and the input base is 10,
	 * cutoff will be set to 214748364 and cutlim to either
	 * 7 (neg==0) or 8 (neg==1), meaning that if we have accumulated
	 * a value > 214748364, or equal but the next digit is > 7 (or 8),
	 * the number is too big, and we will return a range error.
	 *
	 * Set any if any `digits' consumed; make it negative to indicate
	 * overflow.
	 */
	cutoff = neg ? -(unsigned long)LONG_MIN : LONG_MAX;
	cutlim = cutoff % (unsigned long)base;
	cutoff /= (unsigned long)base;
	for (acc = 0, any = 0;; c = *s++) {
		if (isdigit(c))
			c -= '0';
		else if (isalpha(c))
			c -= isupper(c) ? 'A' - 10 : 'a' - 10;
		else
			break;
		if (c >= base)
			break;
               if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim))
			any = -1;
		else {
			any = 1;
			acc *= base;
			acc += c;
		}
	}
	if (any < 0) {
		acc = neg ? LONG_MIN : LONG_MAX;
		errno = ERANGE;
	} else if (neg)
		acc = -acc;
	if (endptr != 0)
		*endptr = (const char *) (any ? (const char *)s - 1 : nptr);
	return (acc);
}
