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

#define NEXT_TAG(tag) \
    (struct multiboot_tag *)((addr_t)tag + ((tag->size + 7) & ~7))
#define FOR_ALL_TAGS(tag, vaddr)                                \
    for (tag = (struct multiboot_tag *) ((addr_t)vaddr + 8);    \
        tag->type != MULTIBOOT_TAG_TYPE_END;                    \
        tag = NEXT_TAG(tag))
#define NEXT_MMAP(mmap, tag) \
    (multiboot_memory_map_t *)((addr_t) mmap + ((struct multiboot_tag_mmap *) tag)->entry_size)
#define FOR_ALL_MMAP(mmap, tag)                                 \
    for (mmap = ((struct multiboot_tag_mmap *) tag)->entries;   \
        (addr_t)mmap < ((addr_t)tag + tag->size);               \
        mmap = NEXT_MMAP(mmap, tag))

KMultibootManager2::KMultibootManager2(void* multiboot_tags_base) :
    KIMultibootManager(multiboot_tags_base)
{
    this->multiboot_tags_base = multiboot_tags_base;
}

kusable_memory_region_t *KMultibootManager2::getMemoryRegions()
{
    struct multiboot_tag *tag;
    void *vaddr = multiboot_tags_base;
    uint8_t nb_regions = 0;

    uint64_t min_address = (uint64_t)multiboot_tags_base;
    uint64_t max_address = (uint64_t)multiboot_tags_base;

    /* First go through the tags and get the number of
    memory regions */
    FOR_ALL_TAGS(tag, vaddr)
    {
        min_address = min(min_address, (uint64_t)tag);
        max_address = max(max_address, (uint64_t)tag + tag->size);
        if (tag->type == MULTIBOOT_TAG_TYPE_MMAP)
        {
            multiboot_memory_map_t *mmap;
            FOR_ALL_MMAP(mmap, tag)
            {
                min_address = min(min_address, (uint64_t)mmap);
                max_address = max(max_address, (uint64_t)mmap + sizeof(*mmap));
                kprintk("Found an availabe memory region of type %d: base_addr = 0x%lx,"
                    " length = 0x%lx\n", mmap->type, mmap->addr, mmap->len);
                if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE)
                {
                    KDBG("Found an availabe memory region: base_addr = 0x%lx,"
                        " length = 0x%lx\n", mmap->addr, mmap->len);
                    nb_regions++;
                }
            }
        }
        else if (tag->type == MULTIBOOT_TAG_TYPE_CMDLINE)
        {
            struct multiboot_tag_string *stag = (struct multiboot_tag_string *)tag;
            int len = strlen(stag->string);
            min_address = min(min_address, (uint64_t)stag->string);
            max_address = max(max_address, (uint64_t)stag->string + len);
        }
        else if (tag->type == MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME)
        {
            struct multiboot_tag_string *stag = (struct multiboot_tag_string *)tag;
            int len = strlen(stag->string);
            min_address = min(min_address, (uint64_t)stag->string);
            max_address = max(max_address, (uint64_t)stag->string + len);
        }
    }

    kprintk("multiboot_tags range from virtual 0x%016lx to 0x%016lx\n", min_address,
        max_address);

    if (nb_regions == 0) return (kusable_memory_region_t *)NULL;

    kusable_memory_region_t *mem_regions = (kusable_memory_region_t*)malloc((nb_regions+1) * sizeof(kusable_memory_region_t *));
    if (!mem_regions) return mem_regions;

    mem_regions[nb_regions].size = 0;
    mem_regions[nb_regions].base_address = 0;
    int idx = 0;
    FOR_ALL_TAGS(tag, vaddr)
    {
        if (tag->type == MULTIBOOT_TAG_TYPE_MMAP)
        {
            multiboot_memory_map_t *mmap;
            FOR_ALL_MMAP(mmap, tag)
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
    void *vaddr = multiboot_tags_base;
    char *commandline = (char*)NULL;

    /* First go through the tags and get the number of
    memory regions */
    FOR_ALL_TAGS(tag, vaddr)
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
    void *vaddr = multiboot_tags_base;
    char *bootloadername = (char*)NULL;

    /* First go through the tags and get the number of
    memory regions */
    FOR_ALL_TAGS(tag, vaddr)
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
