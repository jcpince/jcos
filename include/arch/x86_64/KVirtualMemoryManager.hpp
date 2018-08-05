#ifndef __KVIRTUALMEMORYMANAGER_HPP__
#define __KVIRTUALMEMORYMANAGER_HPP__

#include <mem/KIVirtualMemoryManager.hpp>
#include <arch/x86_64/KCpuHelper.hpp>

typedef struct virtual_address_ {
    uint32_t offset         : 12;
    uint32_t pte            : 9;
    uint32_t pde            : 9;
    uint32_t pdpte          : 9;
    uint32_t pml4e          : 9;
    uint16_t lastbitrepeat;
} __attribute__((packed)) virtual_address_t;

typedef struct cr3_ {
    uint8_t ignored         : 3;
    uint8_t pwt             : 1;
    uint8_t pcd             : 1;
    uint8_t ignored_        : 3;
    uint8_t ignored__       : 4;
    uint32_t pml4_phys_4k   : 28;
    uint32_t zeros          : 24;
} __attribute__((packed)) cr3_t;

typedef struct pml4e_ {
    uint8_t present         : 1;
    uint8_t writeable       : 1;
    uint8_t user_acess      : 1;
    uint8_t pwt             : 1;
    uint8_t pcd             : 1;
    uint8_t accessed        : 1;
    uint8_t ignored         : 1;
    uint8_t reserved        : 1;
    uint8_t ignored_        : 4;
    uint64_t pdpt_phys_4k   : 28;
    uint32_t reserved__     : 12;
    uint32_t ignored__      : 11;
    uint8_t execute_dis     : 1;
} __attribute__((packed)) pml4e_t;

typedef struct pdpte_pde_ {
    uint8_t present         : 1;
    uint8_t writeable       : 1;
    uint8_t user_acess      : 1;
    uint8_t pwt             : 1;
    uint8_t pcd             : 1;
    uint8_t accessed        : 1;
    uint8_t ignored         : 1;
    uint8_t page_size       : 1;
    uint8_t ignored_        : 4;
    uint64_t subt_phys_4k   : 28;
    uint32_t reserved__     : 12;
    uint32_t ignored__      : 11;
    uint8_t execute_dis     : 1;
} __attribute__((packed)) pdpte_t, pde_t;

typedef struct pte_ {
    uint8_t present         : 1;
    uint8_t writeable       : 1;
    uint8_t user_acess      : 1;
    uint8_t pwt             : 1;
    uint8_t pcd             : 1;
    uint8_t accessed        : 1;
    uint8_t dirty           : 1;
    uint8_t pat             : 1;
    uint8_t global          : 1;
    uint8_t ignored         : 3;
    uint64_t page_phys_4k   : 28;
    uint8_t reserved        : 12;
    uint8_t ignored_        : 4;
    uint8_t ignored__       : 3;
    uint8_t protection_key  : 4;
    uint8_t execute_dis     : 1;
} __attribute__((packed)) pte_t;

class KVirtualMemoryManager : public KIVirtualMemoryManager
{
private:
    KCpuHelper *cpuH;

public:
    KVirtualMemoryManager();
    ~KVirtualMemoryManager();

    void dump_virtual_mapping(vmap_verbosity_t verbosity);
    void dump_cr3_decoded(cr3_t *cr3);
    void dump_pml4_decoded(pml4e_t *pml4e, uint32_t index);
    void dump_pdpte_decoded(pdpte_t *pdpte, uint32_t index);
    void dump_pde_decoded(pde_t *pde, uint32_t index);
    void dump_pte_decoded(pte_t *pte, uint32_t index);
    addr_t virt2phys(void *vaddr);
    void *phys2virt(addr_t paddr);

    bool add_virtual_mapping(addr_t pstart, void *vstart, uint32_t range);
    bool remove_virtual_mapping(addr_t pstart, void *vstart, uint32_t range);
};

#endif /* __KVIRTUALMEMORYMANAGER_HPP__ */
