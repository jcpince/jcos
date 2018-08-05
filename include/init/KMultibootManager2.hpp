#ifndef __KMULTIBOOTMANAGER2_HPP__
#define __KMULTIBOOTMANAGER2_HPP__

#include <init/KIMultibootManager.hpp>

class KMultibootManager2 : public KIMultibootManager
{
private:
	KMultibootManager2() : KIMultibootManager(0), multiboot_tags_base(0) {}
	~KMultibootManager2() {}

	void* multiboot_tags_base;

public:
	KMultibootManager2(void* multiboot_tags_base);
	kusable_memory_region_t *getMemoryRegions();
	char *getCommandLine();
	char *getBootloaderName();
};

#endif /* __KMULTIBOOTMANAGER2_HPP__ */
