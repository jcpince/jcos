#include <init/KMultibootManager2.hpp>
#include <init/multiboot2.h>
#include <stdlib.h>
#include <string.h>

#undef DEBUG
//#define DEBUG
#ifdef DEBUG
#include <KOStream.hpp>
#define KDBG kstd::kout.printf
#else
#define KDBG(...)
#endif

KMultibootManager2::KMultibootManager2(addr_t multiboot_tags_base) :
	KIMultibootManager(multiboot_tags_base)
{
	this->multiboot_tags_base = multiboot_tags_base;
}

kusable_memory_region_t *KMultibootManager2::getMemoryRegions()
{
	struct multiboot_tag *tag;
	void *paddr = (void*)multiboot_tags_base;
	uint8_t nb_regions = 0;
	
	/* First go through the tags and get the number of
		memory regions */
	for (tag = (struct multiboot_tag *) ((addr_t)paddr + 8);
		tag->type != MULTIBOOT_TAG_TYPE_END;
		tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag
		+ ((tag->size + 7) & ~7)))
	{
		if (tag->type == MULTIBOOT_TAG_TYPE_MMAP)
		{
			multiboot_memory_map_t *mmap;
			for (mmap = ((struct multiboot_tag_mmap *) tag)->entries;
				(multiboot_uint8_t *) mmap < (multiboot_uint8_t *) tag + tag->size;
				mmap = (multiboot_memory_map_t *)((addr_t) mmap
				+ ((struct multiboot_tag_mmap *) tag)->entry_size))
			{
				if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE)
				{
					KDBG("Found an availabe memory region: base_addr = 0x%lx,"
						" length = 0x%lx\n", mmap->addr, mmap->len);
					nb_regions++;
				}
			}
		}
	}
	
	if (nb_regions == 0) return (kusable_memory_region_t *)NULL;
	
	kusable_memory_region_t *mem_regions = (kusable_memory_region_t*)malloc((nb_regions+1) * sizeof(kusable_memory_region_t *));
	if (!mem_regions) return mem_regions;
	
	mem_regions[nb_regions].size = 0;
	mem_regions[nb_regions].base_address = 0;
	int idx = 0;
	
	for (tag = (struct multiboot_tag *) ((addr_t)paddr + 8);
		tag->type != MULTIBOOT_TAG_TYPE_END;
		tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag
		+ ((tag->size + 7) & ~7)))
	{
		if (tag->type == MULTIBOOT_TAG_TYPE_MMAP)
		{
			multiboot_memory_map_t *mmap;
			for (mmap = ((struct multiboot_tag_mmap *) tag)->entries;
				(multiboot_uint8_t *) mmap < (multiboot_uint8_t *) tag + tag->size;
				mmap = (multiboot_memory_map_t *)((addr_t) mmap
				+ ((struct multiboot_tag_mmap *) tag)->entry_size))
			{
				if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE)
				{
					mem_regions[idx].base_address = mmap->addr;
					mem_regions[idx++].size = mmap->len;
				}
			}
		}
	}
	return mem_regions;
}

char *KMultibootManager2::getCommandLine()
{
	struct multiboot_tag *tag;
	void *paddr = (void*)multiboot_tags_base;
	char *commandline = (char*)NULL;
	
	/* First go through the tags and get the number of
		memory regions */
	for (tag = (struct multiboot_tag *) ((addr_t)paddr + 8);
		tag->type != MULTIBOOT_TAG_TYPE_END;
		tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag
		+ ((tag->size + 7) & ~7)))
	{
		if (tag->type == MULTIBOOT_TAG_TYPE_CMDLINE)
		{
			int len = strlen(((struct multiboot_tag_string *) tag)->string);
			commandline = (char*)malloc(len*sizeof(char));
			return strncpy(commandline, ((struct multiboot_tag_string *) tag)->string, len);
		}
	}
	
	return commandline;
}

char *KMultibootManager2::getBootloaderName()
{
	struct multiboot_tag *tag;
	void *paddr = (void*)multiboot_tags_base;
	char *bootloadername = (char*)NULL;
	
	/* First go through the tags and get the number of
		memory regions */
	for (tag = (struct multiboot_tag *) ((addr_t)paddr + 8);
		tag->type != MULTIBOOT_TAG_TYPE_END;
		tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag
		+ ((tag->size + 7) & ~7)))
	{
		if (tag->type == MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME)
		{
			int len = strlen(((struct multiboot_tag_string *) tag)->string);
			bootloadername = (char*)malloc(len*sizeof(char));
			return strncpy(bootloadername, ((struct multiboot_tag_string *) tag)->string, len);
		}
	}
	
	return bootloadername;
}

