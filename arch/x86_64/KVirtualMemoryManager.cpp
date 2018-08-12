#include <arch/x86_64/KVirtualMemoryManager.hpp>

#include <stdlib.h>

extern const uint64_t KERNEL_VIRT_ADDR;
extern const uint64_t KERNEL_PHYS_MASK;
extern const uint64_t KERNEL_PHYS_ADDR;

#define LINEAR_0_OFFSET         ((uint64_t)&KERNEL_VIRT_ADDR)
#define LINEAR_MASK             ((uint64_t)&KERNEL_PHYS_MASK)
#define MAPPING_TABLES_SIZE     512UL
#define PHYS_PAGE_GRANULARITY   (4UL * KB)

KIVirtualMemoryManager *GetVirtualMemoryManager()
{
    static KIVirtualMemoryManager *kvmem = (KIVirtualMemoryManager*)NULL;
    if (!kvmem) kvmem = new KVirtualMemoryManager();

    return kvmem;
}

KVirtualMemoryManager::KVirtualMemoryManager()
{
    cpuH = GetX86CpuHelper();
}

KVirtualMemoryManager::~KVirtualMemoryManager()
{

}

void KVirtualMemoryManager::dump_cr3_decoded(cr3_t *cr3)
{
    kprintk("CR3: PWT(%d), PCD(%d), pml4_phys_address(0x%016lx)\n",
        cr3->pwt, cr3->pcd, cr3->pml4_phys_4k*PHYS_PAGE_GRANULARITY);
}

void KVirtualMemoryManager::dump_pml4_decoded(pml4e_t *pml4e, uint32_t index)
{
    kprintk("pml4(%p): index 0x%03x, P(%d), R/W(%d), U/S(%d), PWT(%d), PCD(%d), A(%d), XD(%d), pdpt_phys_address(0x%016x)\n",
        pml4e, index, pml4e->present, pml4e->writeable, pml4e->user_acess, pml4e->pwt,
        pml4e->pcd, pml4e->accessed, pml4e->execute_dis, pml4e->pdpt_phys_4k*PHYS_PAGE_GRANULARITY);
}

void KVirtualMemoryManager::dump_pdpte_decoded(pdpte_t *pdpte, uint32_t index)
{
    kprintk("pdpte(%p): index 0x%03x, P(%d), R/W(%d), U/S(%d), PWT(%d), PCD(%d), A(%d), PS(%d), XD(%d), pd_phys_address(0x%016x)\n",
        pdpte, index, pdpte->present, pdpte->writeable, pdpte->user_acess, pdpte->pwt,
        pdpte->pcd, pdpte->accessed, pdpte->subt_phys_4k, pdpte->execute_dis, pdpte->subt_phys_4k*PHYS_PAGE_GRANULARITY);
}

void KVirtualMemoryManager::dump_pde_decoded(pde_t *pde, uint32_t index)
{
    kprintk("pde(%p): index 0x%03x, P(%d), R/W(%d), U/S(%d), PWT(%d), PCD(%d), A(%d), PS(%d), XD(%d), pt_phys_address(0x%016x)\n",
        pde, index, pde->present, pde->writeable, pde->user_acess, pde->pwt,
        pde->pcd, pde->accessed, pde->subt_phys_4k, pde->execute_dis, pde->subt_phys_4k*PHYS_PAGE_GRANULARITY);
}

void KVirtualMemoryManager::dump_pte_decoded(pte_t *pte, uint32_t index)
{
    kprintk("pte(%p): index 0x%03x, P(%d), R/W(%d), U/S(%d), PWT(%d), PCD(%d), A(%d), DIRTY(%d), PAT(%d), GLOB(%d), PROTK(%d), XD(%d), page_phys_address(0x%016x)\n",
        pte, index, pte->present, pte->writeable, pte->user_acess, pte->pwt,
        pte->pcd, pte->accessed, pte->dirty, pte->pat, pte->global,
        pte->protection_key, pte->execute_dis, pte->page_phys_4k*PHYS_PAGE_GRANULARITY);
}

void KVirtualMemoryManager::dump_virtual_mapping(vmap_verbosity_t verbosity)
{
    kprintk("Virtual memory mapping:\n");
    kprintk("KERNEL_VIRT_ADDR: %p\n", &KERNEL_VIRT_ADDR);
    kprintk("KERNEL_PHYS_MASK: %p\n", &KERNEL_PHYS_MASK);
    kprintk("KERNEL_PHYS_ADDR: %p\n", &KERNEL_PHYS_ADDR);
    uint64_t cr0, cr3, cr4, efer;
    cr0 = cpuH->read_cr0();
    cr3 = cpuH->read_cr3();
    cr4 = cpuH->read_cr4();
    efer = cpuH->read_msr(X86_64_MSR_EFER);
    kprintk("cr0.PG:              %d\n", ISBITSET(cr0, X86_CR0_PG));
    kprintk("cr4.PAE:             %d\n", ISBITSET(cr4, X86_CR4_PAE));
    kprintk("cr4.PCIDE:           %d\n", ISBITSET(cr4, X86_CR4_PCIDE));
    kprintk("efer.LME:            %d\n", ISBITSET(efer, X86_64_MSR_EFER_LME));
    register uint32_t value asm("eax") = 0x80000008;
    __asm__ __volatile__("cpuid");
    /* Store in a stack variable because eaxwill be changed in subsequent code */
    uint32_t v = value;
    kprintk("cpuid(0x80000008):   0x%04x\n", v);
    uint32_t phys_address_size = v & 0xff;
    kprintk("phys_address_size:   %02d\n", phys_address_size);
    uint32_t lin_address_size = (v >> 8) & 0xff;
    kprintk("lin_address_size:    %02d\n", lin_address_size);
    cr3_t *pcr3 = (cr3_t *)&cr3;
    dump_cr3_decoded(pcr3);
    pml4e_t *pml4e = (pml4e_t *)phys2virt(pcr3->pml4_phys_4k * PHYS_PAGE_GRANULARITY);

    /* stats */
    virtual_address_t v_, *virt_address = &v_;
    uint64_t *virt_addr64 = (uint64_t*)virt_address;
    uint64_t section_phys_start = 0, section_virt_start = 0, section_size = 0;
    uint64_t min_tables_address = ~0UL, max_tables_address = 0;
    virt_address->offset = 0;

    for (uint32_t idx = 0 ; idx < MAPPING_TABLES_SIZE ; idx++, pml4e++)
    {
        if (!pml4e->present) continue;
        virt_address->pml4e = idx;
        if (idx > 0x100) virt_address->lastbitrepeat = 0xffff;
        else virt_address->lastbitrepeat = 0x0;
        if (verbosity == VM_DUMP_TABLES_DUPLICATES ||
            (!pml4e->ignored && verbosity == VM_DUMP_TABLES))
            dump_pml4_decoded(pml4e, idx);
        pml4e->ignored = 1;
        min_tables_address = min(min_tables_address, pml4e->pdpt_phys_4k * PHYS_PAGE_GRANULARITY);
        max_tables_address = max(max_tables_address, pml4e->pdpt_phys_4k * PHYS_PAGE_GRANULARITY);
        pdpte_t *pdpte = (pdpte_t *)phys2virt(pml4e->pdpt_phys_4k * PHYS_PAGE_GRANULARITY);
        for (uint32_t jdx = 0 ; jdx < MAPPING_TABLES_SIZE ; jdx++, pdpte++)
        {
            if (!pdpte->present) continue;
            virt_address->pdpte = jdx;
            if (verbosity == VM_DUMP_TABLES_DUPLICATES ||
                (!pdpte->ignored && verbosity == VM_DUMP_TABLES))
                dump_pdpte_decoded(pdpte, jdx);
            pdpte->ignored = 1;
            min_tables_address = min(min_tables_address, pdpte->subt_phys_4k * PHYS_PAGE_GRANULARITY);
            max_tables_address = max(max_tables_address, pdpte->subt_phys_4k * PHYS_PAGE_GRANULARITY);
            pde_t *pde = (pde_t *)phys2virt(pdpte->subt_phys_4k * PHYS_PAGE_GRANULARITY);
            for (uint32_t kdx = 0 ; kdx < MAPPING_TABLES_SIZE ; kdx++, pde++)
            {
                if (!pde->present) continue;
                virt_address->pde = kdx;
                if (verbosity == VM_DUMP_TABLES_DUPLICATES ||
                    (!pde->ignored && verbosity == VM_DUMP_TABLES))
                    dump_pde_decoded(pde, kdx);
                pde->ignored = 1;
                min_tables_address = min(min_tables_address, pde->subt_phys_4k * PHYS_PAGE_GRANULARITY);
                max_tables_address = max(max_tables_address, pde->subt_phys_4k * PHYS_PAGE_GRANULARITY);
                pte_t *pte = (pte_t *)phys2virt(pde->subt_phys_4k * PHYS_PAGE_GRANULARITY);
                for (uint32_t ldx = 0 ; ldx < MAPPING_TABLES_SIZE ; ldx++, pte++)
                {
                    if (!pte->present) continue;
                    virt_address->pte = ldx;
                    if (verbosity == VM_DUMP_TABLES_DUPLICATES ||
                        (!pte->ignored && verbosity == VM_DUMP_TABLES))
                        dump_pte_decoded(pte, ldx);
                    pte->ignored = 1;
                    uint64_t new_page_phys = pte->page_phys_4k * PHYS_PAGE_GRANULARITY;
                    uint64_t new_page_virt = *virt_addr64;
                    if (((section_phys_start + section_size + PHYS_PAGE_GRANULARITY) == new_page_phys) &&
                            ((section_virt_start + section_size + PHYS_PAGE_GRANULARITY) == new_page_virt))
                    {
                        /* contiguous block */
                        section_size += PHYS_PAGE_GRANULARITY;
                        continue;
                    }
                    if (section_size)
                    {
                        kprintk("Pages from 0x%016lx to 0x%016lx mapped from 0x%016lx to 0x%016lx -- section size 0x%x.\n",
                            section_phys_start, section_phys_start + section_size,
                            section_virt_start, section_virt_start + section_size,
                            section_size + PHYS_PAGE_GRANULARITY
                        );
                    }
                    section_phys_start = new_page_phys;
                    section_virt_start = new_page_virt;
                    section_size = 0;
                }
            }
        }
    }
    /* last section */
    kprintk("Pages from 0x%016lx to 0x%016lx mapped from 0x%016lx to 0x%016lx -- section size 0x%x.\n",
        section_phys_start, section_phys_start + section_size,
        section_virt_start, section_virt_start + section_size,
        section_size + PHYS_PAGE_GRANULARITY);
    // kprintk("Pages from 0x%016lx to 0x%016lx.\n",
    //     section_phys_start, previous_page_end);
    kprintk("PML4 tables ranges from 0x%016lx to 0x%016lx.\n", min_tables_address, max_tables_address);

    /* reset the ignored flags */
    pml4e = (pml4e_t *)phys2virt(pcr3->pml4_phys_4k * PHYS_PAGE_GRANULARITY);
    for (uint32_t idx = 0 ; idx < MAPPING_TABLES_SIZE ; idx++, pml4e++)
    {
        pml4e->ignored = 0;
        if (!pml4e->present) continue;
        pdpte_t *pdpte = (pdpte_t *)phys2virt(pml4e->pdpt_phys_4k * PHYS_PAGE_GRANULARITY);
        for (uint32_t jdx = 0 ; jdx < MAPPING_TABLES_SIZE ; jdx++, pdpte++)
        {
            pdpte->ignored = 0;
            if (!pdpte->present) continue;
            pde_t *pde = (pde_t *)phys2virt(pdpte->subt_phys_4k * PHYS_PAGE_GRANULARITY);
            for (uint32_t kdx = 0 ; kdx < MAPPING_TABLES_SIZE ; kdx++, pde++)
            {
                pde->ignored = 0;
                if (!pde->present) continue;
                pte_t *pte = (pte_t *)phys2virt(pde->subt_phys_4k * PHYS_PAGE_GRANULARITY);
                for (uint32_t ldx = 0 ; ldx < MAPPING_TABLES_SIZE ; ldx++, pte++)
                {
                    pte->ignored = 0;
                }
            }
        }
    }
    kprintk("End of virtual memory mapping\n");
}

addr_t KVirtualMemoryManager::virt2phys(void *vaddr)
{
    return ((addr_t)vaddr & LINEAR_MASK);
}

void *KVirtualMemoryManager::phys2virt(addr_t paddr)
{
    return (void*)(paddr + LINEAR_0_OFFSET);
}

bool KVirtualMemoryManager::add_virtual_mapping(addr_t __attribute__((unused)) pstart, void __attribute__((unused)) *vstart, uint32_t __attribute__((unused)) range)
{
    return false;
}

bool KVirtualMemoryManager::remove_virtual_mapping(addr_t __attribute__((unused)) pstart, void __attribute__((unused)) *vstart, uint32_t __attribute__((unused)) range)
{
    return false;
}
