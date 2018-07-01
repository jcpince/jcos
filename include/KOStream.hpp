#ifndef __OSTREAM_H__
#define __OSTREAM_H__

#include <stdint.h>
#include <Video.h>

#define _L(chain) (char*)(chain)

namespace kstd
{
//---------------OStream:------------------------

	const char endl = '\n';

	class KOStream : public Video
	{
		public:
			KOStream& operator<<(const char *cp);
    		KOStream& operator<<(char *cp);
			KOStream& operator<<(char c);
			KOStream& operator<<(int32_t i32);
			KOStream& operator<<(int64_t i64);
			KOStream& operator<<(uint32_t i32);
			KOStream& operator<<(uint64_t i64);
			KOStream& operator<<(unsigned char *cq);
			KOStream& operator<<(unsigned char c1);

			void printf(const char *fmt, ...);

		private:
	};

	extern KOStream kout;
}

#endif
