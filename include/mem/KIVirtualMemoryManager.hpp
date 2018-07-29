#ifndef __KIVIRTUALMEMORYMANAGER_HPP__
#define __KIVIRTUALMEMORYMANAGER_HPP__

#include <stdint.h>

class KIVirtualMemoryManager
{
public:
    virtual void dump_virtual_mapping() = 0;
    virtual addr_t virt2phys(void *vaddr) = 0;
    virtual void *phys2virt(addr_t paddr) = 0;

    virtual bool add_virtual_mapping(addr_t pstart, void *vstart, uint32_t range) = 0;
    virtual bool remove_virtual_mapping(addr_t pstart, void *vstart, uint32_t range) = 0;
};

KIVirtualMemoryManager *GetVirtualMemoryManager();

#endif /* __KIVIRTUALMEMORYMANAGER_HPP__ */
