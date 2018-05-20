#include <KOStream.hpp>
#include <stdio.h>
#include <stdarg.h>

extern "C" void VideoPuts(const char *buffer)
{
	kstd::kout << (char *)buffer;
}

namespace kstd
{
	KOStream kout;		//KOStream kout

	KOStream& KOStream::operator<<(char *cp)
	{
		write(cp) ;
		return *this;
	}

	KOStream& KOStream::operator<<(char c)
	{
		put(c) ;
		return *this;
	}

	KOStream& KOStream::operator<<(int32_t i32)
	{
		char buf[64];
		sprintf(buf, "%d", i32);
		write(&buf[0]) ;
		return *this;
	}

	KOStream& KOStream::operator<<(int64_t i64)
	{
		char buf[64];
		sprintf(buf, "%ld", i64);
		write(&buf[0]) ;
		return *this;
	}

	KOStream& KOStream::operator<<(uint32_t ui32)
	{
		char buf[64];
		sprintf(buf, "%u", ui32);
		write(&buf[0]) ;
		return *this;
	}

	KOStream& KOStream::operator<<(uint64_t ui64)
	{
		char buf[64];
		sprintf(buf, "%lu", ui64);
		write(&buf[0]) ;
		return *this;
	}

	KOStream& KOStream::operator<<(unsigned char *cq)
	{
		write((char *)cq) ;
		return *this;
	}

	KOStream& KOStream::operator<<(unsigned char c1)
	{
		put((char)c1) ;
		return *this;
	}
	
	void KOStream::printf(const char *fmt, ...)
	{
		va_list args;
		char printbuffer[CFG_PBSIZE];

		va_start (args, fmt);

		/* For this to work, printbuffer must be larger than
		 * anything we ever want to print.
		 */
		vsprintf (printbuffer, fmt, args);
		va_end (args);

		/* Print the string */
		write((char *)printbuffer) ;
	}
}

