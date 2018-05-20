#ifndef __KMEMPAGEHDLR_HPP__
#define __KMEMPAGEHDLR_HPP__

#include <stdint.h>
#include <stdlib.h>
#include <arch/x86_64/system.h>


typedef struct free_pages_bitmap {
	uint64_t bitmap;
	addr_t base;
	struct free_pages_bitmap *next;
} free_pages_bitmap_t;

class KMemPageHdlr
{
protected:
	free_pages_bitmap_t *fpBmps;

	uint32_t findFreeRange(free_pages_bitmap_t &bmp,
		uint32_t rangewidth);
	free_pages_bitmap_t *page2bmp(addr_t page);
	uint32_t page2pageinbmp(free_pages_bitmap_t *bmp, addr_t page);
	void freePages(free_pages_bitmap_t &bmp, uint32_t slotstart, uint32_t slotwidth);
	void usePages(free_pages_bitmap_t &bmp, uint32_t slotstart, uint32_t slotwidth);

public:
	KMemPageHdlr() {this->fpBmps = 0;}
	~KMemPageHdlr();
	
	addr_t getFreePage();
	addr_t getConsecutivesFreePages(uint32_t nbpages);
	void releasePages(addr_t page, uint32_t nbpages);
	void addMemorySegment();
};

#endif /* __KMEMPAGEHDLR_HPP__ */
