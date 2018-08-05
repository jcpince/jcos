#ifndef __KMULTIBOOTMANAGER_HPP__
#define __KMULTIBOOTMANAGER_HPP__

#include <stdint.h>

#define MB2_BTL_MAGIC	0x36D76289
#define MB1_HDR_MAGIC	0x1BADB002

typedef struct {
	addr_t		base_address;
	uint64_t	size;
} kusable_memory_region_t;

class KIMultibootManager
{
private:
	KIMultibootManager() {}
public:
	KIMultibootManager(void* multiboot_base) {}
	virtual ~KIMultibootManager() {}

	virtual kusable_memory_region_t *getMemoryRegions() = 0;
	virtual char *getCommandLine() = 0;
	virtual char *getBootloaderName() = 0;
};

#endif /* __KMULTIBOOTMANAGER_HPP__ */
