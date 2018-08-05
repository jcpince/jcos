#ifndef __KIVIRTUALMEMORYMANAGER_HPP__
#define __KIVIRTUALMEMORYMANAGER_HPP__

#include <stdint.h>

typedef enum
{
    VM_DUMP_MAPPING_ONLY,
    VM_DUMP_TABLES,
    VM_DUMP_TABLES_DUPLICATES,
} vmap_verbosity_t;

class KIVirtualMemoryManager
{
public:
    virtual void dump_virtual_mapping(vmap_verbosity_t verbosity = VM_DUMP_MAPPING_ONLY) = 0;
    virtual addr_t virt2phys(void *vaddr) = 0;
    virtual void *phys2virt(addr_t paddr) = 0;

    virtual bool add_virtual_mapping(addr_t pstart, void *vstart, uint32_t range) = 0;
    virtual bool remove_virtual_mapping(addr_t pstart, void *vstart, uint32_t range) = 0;
};

KIVirtualMemoryManager *GetVirtualMemoryManager();

#endif /* __KIVIRTUALMEMORYMANAGER_HPP__ */
