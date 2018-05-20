#include <KMemPageHdlr.hpp>

uint32_t KMemPageHdlr::findFreeRange(free_pages_bitmap_t &bmp,
	uint32_t rangewidth)
{
	uint64_t mask = (2^rangewidth)-1;
	uint64_t map = bmp.bitmap;
	
	for (uint32_t i = 0 ; i < 8*sizeof(bmp.bitmap) ; i++)
	{
		if ((map & mask) == mask)
			return i;
		map >>= 1;
	}
	return -1;
}

addr_t KMemPageHdlr::getFreePage()
{
	return getConsecutivesFreePages(1);
}

addr_t KMemPageHdlr::getConsecutivesFreePages(uint32_t nbpages)
{
	free_pages_bitmap_t *curbmp = fpBmps;
	
	while (curbmp)
	{
		if (curbmp->bitmap)
		{
			uint32_t idx = findFreeRange(*curbmp, nbpages);
			if (idx > 0)
			{
				usePages(*curbmp, page2pageinbmp(curbmp, idx), nbpages);
				return curbmp->base + idx*PAGESZ;
			}
		}
		curbmp = curbmp->next;
	}

	return -1;
}

void KMemPageHdlr::releasePages(addr_t page, uint32_t nbpages)
{
	free_pages_bitmap_t *bmp = page2bmp(page);
	if (!bmp) return;
	
	uint32_t pageinbmp = page2pageinbmp(bmp, page);
	
	freePages(*bmp, pageinbmp, nbpages);
}

void KMemPageHdlr::freePages(free_pages_bitmap_t &bmp, uint32_t slotstart, uint32_t slotwidth)
{
	uint64_t mask = ((2^slotwidth)-1) << slotstart;
	bmp.bitmap |= mask;
}

void KMemPageHdlr::usePages(free_pages_bitmap_t &bmp, uint32_t slotstart, uint32_t slotwidth)
{
	uint64_t mask = ~(((2^slotwidth)-1) << slotstart);
	bmp.bitmap &= mask;
}

free_pages_bitmap_t *KMemPageHdlr::page2bmp(addr_t page)
{
	free_pages_bitmap_t *curbmp = fpBmps;
	
	while (curbmp)
	{
		if (curbmp->base <= page && page <= (curbmp->base+64*PAGESZ))
		{
			return curbmp;
		}
		curbmp = curbmp->next;
	}

	return (free_pages_bitmap_t *)NULL;
}

uint32_t KMemPageHdlr::page2pageinbmp(free_pages_bitmap_t *bmp, addr_t page)
{
	return (page-bmp->base)/PAGESZ;
}
