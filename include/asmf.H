#ifndef _asmf_H_
#define _asmf_H_

#include <stdint.h>

// Assembly routines //
//-------------------//

inline static uint8_t inportb(uint16_t port)
{
	register unsigned char r;

   	asm volatile
	(
		"inb %%dx, %%al\n\t"
		: "=a" (r)
		: "d" (port)
	);

   	return (r);
}

inline static void outportb(uint16_t port, uint8_t data)
{
	asm volatile
	(
		"outb %%al, %%dx\n\t"
		:
		: "a" (data), "d" (port)
	);
}

inline static uint16_t inportw(uint16_t port)
{
	register unsigned short r;

   	asm volatile
	(
		"inw %%dx, %%ax\n\t"
		: "=a" (r)
		: "d" (port)
	);

   	return (r);
}

inline static void outportw(uint16_t port, uint16_t data)
{
	asm volatile
	(
		"outw %%ax, %%dx\n\t"
		:
		: "a" (data), "d" (port)
	);
}



inline static void enable()
{
	asm volatile
	(
		"sti"
		:
		:
	);
}

inline static void disable()
{
	asm volatile
	(
		"cli"
		:
		:
	);
}

inline static void halt()
{
	asm volatile
	(
		"hlt"
		:
		:
	);
} /*Halts cpu*/


  /*inline static void pokeb(unsigned short selector, unsigned long offset, unsigned char value)
{
	asm volatile
	(
		"movw %w0,%%fs\n"
		"	.byte 0x64 \n"
		"	movb %b1,(%k2)"
		:
		: "rm" (selector), "qi" (value), "r" (offset)
	);
}

inline static void pokew(unsigned short selector, unsigned long offset, unsigned short value)
{
	asm volatile
	(
		"movw %w0,%%fs \n"
		"	.byte 0x64 \n"
		"	movw %w1,(%k2)"
		:
		: "rm" (selector), "ri" (value), "r" (offset)
	);
}

inline static void pokel(unsigned short selector, unsigned long offset, unsigned long value)
{
	asm volatile
	(
		"movw %w0,%%fs \n"
		"	.byte 0x64 \n"
		"	movl %k1,(%k2)"
		:
		: "rm" (selector), "ri" (value), "r" (offset)
	);
}

inline static unsigned char peekb(unsigned short selector, unsigned long offset)
{
	unsigned char result;
	asm volatile
	(
		"movw %w1,%%fs \n"
		"	.byte 0x64 \n"
		"	movb (%k2),%b0"
		: "=q" (result)
		: "rm" (selector), "r" (offset)
	);
	return result;
}

inline static unsigned short peekw(unsigned short selector, unsigned long offset)
{
	unsigned short result;
	asm volatile
	(
		"movw %w1, %%fs \n"
		"	.byte 0x64 \n"
		"	movw (%k2),%w0 \n"
		: "=r" (result)
		: "rm" (selector), "r" (offset)
	);
	return result;
}

inline static unsigned long peekl(unsigned short selector, unsigned long offset)
{
	unsigned long result;
	asm volatile
	(
		"movw %w1,%%fs\n"
		"	.byte 0x64\n"
		"	movl (%k2),%k0"
		: "=r" (result)
		: "rm" (selector), "r" (offset)
	);
	return result;
}*/


#endif
