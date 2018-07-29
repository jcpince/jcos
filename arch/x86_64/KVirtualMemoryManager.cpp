#include <arch/x86_64/KVirtualMemoryManager.hpp>

#include <stdlib.h>

extern const uint64_t KERNEL_VIRT_ADDR;
extern const uint64_t KERNEL_PHYS_MASK;
extern const uint64_t KERNEL_PHYS_ADDR;

#define min(a, b) (a < b ? a : b)
#define max(a, b) (a > b ? a : b)

#define LINEAR_0_OFFSET ((uint64_t)&KERNEL_VIRT_ADDR)
#define LINEAR_MASK     ((uint64_t)&KERNEL_PHYS_MASK)

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
        cr3->pwt, cr3->pcd, cr3->pml4_phys_4k*4*KB);
}

void KVirtualMemoryManager::dump_pml4_decoded(pml4e_t *pml4e, uint32_t index)
{
    kprintk("pml4(%p): index 0x%03x, P(%d), R/W(%d), U/S(%d), PWT(%d), PCD(%d), A(%d), XD(%d), pdpt_phys_address(0x%016x)\n",
        pml4e, index, pml4e->present, pml4e->writeable, pml4e->user_acess, pml4e->pwt,
        pml4e->pcd, pml4e->accessed, pml4e->execute_dis, pml4e->pdpt_phys_4k*4*KB);
}

void KVirtualMemoryManager::dump_pdpte_decoded(pdpte_t *pdpte, uint32_t index)
{
    kprintk("pdpte(%p): index 0x%03x, P(%d), R/W(%d), U/S(%d), PWT(%d), PCD(%d), A(%d), PS(%d), XD(%d), pd_phys_address(0x%016x)\n",
        pdpte, index, pdpte->present, pdpte->writeable, pdpte->user_acess, pdpte->pwt,
        pdpte->pcd, pdpte->accessed, pdpte->page_size, pdpte->execute_dis, pdpte->subt_phys_4k*4*KB);
}

void KVirtualMemoryManager::dump_pde_decoded(pde_t *pde, uint32_t index)
{
    kprintk("pde(%p): index 0x%03x, P(%d), R/W(%d), U/S(%d), PWT(%d), PCD(%d), A(%d), PS(%d), XD(%d), pt_phys_address(0x%016x)\n",
        pde, index, pde->present, pde->writeable, pde->user_acess, pde->pwt,
        pde->pcd, pde->accessed, pde->page_size, pde->execute_dis, pde->subt_phys_4k*4*KB);
}

void KVirtualMemoryManager::dump_pte_decoded(pte_t *pte, uint32_t index)
{
    kprintk("pte(%p): index 0x%03x, P(%d), R/W(%d), U/S(%d), PWT(%d), PCD(%d), A(%d), DIRTY(%d), PAT(%d), GLOB(%d), PROTK(%d), XD(%d), page_phys_address(0x%016x)\n",
        pte, index, pte->present, pte->writeable, pte->user_acess, pte->pwt,
        pte->pcd, pte->accessed, pte->dirty, pte->pat, pte->global,
        pte->protection_key, pte->execute_dis, pte->page_phys_4k*4*KB);
}

void KVirtualMemoryManager::dump_virtual_mapping()
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
    pml4e_t *pml4e = (pml4e_t *)phys2virt(pcr3->pml4_phys_4k * 4 * KB);
    virtual_address_t current_address_, *current_address = &current_address_;
    uint64_t *page_start = (uint64_t *)current_address, section_start = 0, previous_page_end = 0;
    uint64_t min_address = pcr3->pml4_phys_4k * 4 * KB, max_address = pcr3->pml4_phys_4k * 4 * KB;
    for (uint32_t idx = 0 ; idx < 512 ; idx++, pml4e++)
    {
        if (!pml4e->present) continue;
        if (pml4e->ignored)
        {
            kprintk("pml4e(%p) already dumped.\n", pml4e);
            continue;
        }
        current_address->pml4e = idx;
        pml4e->ignored = 1;
        dump_pml4_decoded(pml4e, idx);
        min_address = min(min_address, pml4e->pdpt_phys_4k * 4U * KB);
        max_address = max(max_address, pml4e->pdpt_phys_4k * 4U * KB);
        pdpte_t *pdpte = (pdpte_t *)phys2virt(pml4e->pdpt_phys_4k * 4U * KB);
        for (uint32_t jdx = 0 ; jdx < 512 ; jdx++, pdpte++)
        {
            if (!pdpte->present) continue;
            if (pdpte->ignored)
            {
                kprintk("pdpte(%p) already dumped.\n", pdpte);
                continue;
            }
            current_address->pdpte = jdx;
            pdpte->ignored = 1;
            dump_pdpte_decoded(pdpte, jdx);
            min_address = min(min_address, pdpte->subt_phys_4k * 4U * KB);
            max_address = max(max_address, pdpte->subt_phys_4k * 4U * KB);
            pde_t *pde = (pde_t *)phys2virt(pdpte->subt_phys_4k * 4U * KB);
            for (uint32_t kdx = 0 ; kdx < 512 ; kdx++, pde++)
            {
                if (!pde->present) continue;
                if (pde->ignored)
                {
                    //kprintk("pde(%p) already dumped.\n", pde);
                    continue;
                }
                current_address->pde = kdx;
                pde->ignored = 1;
                dump_pde_decoded(pde, kdx);
                min_address = min(min_address, pde->subt_phys_4k * 4U * KB);
                max_address = max(max_address, pde->subt_phys_4k * 4U * KB);
                pte_t *pte = (pte_t *)phys2virt(pde->subt_phys_4k * 4U * KB);
                for (uint32_t ldx = 0 ; ldx < 512 ; ldx++, pte++)
                {
                    if (!pte->present) continue;
                    if (pte->ignored)
                    {
                        //kprintk("pte(%p) already dumped.\n", pte);
                        continue;
                    }
                    current_address->pte = ldx;
                    pte->ignored = 1;
                    //dump_pte_decoded(pte, ldx);
                    if (previous_page_end == 0) previous_page_end = *page_start-1;
                    if (previous_page_end+1 == *page_start)
                    {
                        /* contiguous block */
                        previous_page_end = *page_start+0xfff;
                        continue;
                    }
                    kprintk("Pages from 0x%016lx to 0x%016lx.\n", section_start, previous_page_end);
                    section_start = *page_start;
                    previous_page_end = *page_start+0xfff;
                }
            }
        }
    }
    kprintk("Pages from 0x%016lx to 0x%016lx.\n", section_start, previous_page_end);
    kprintk("PML4 ranges from 0x%016lx to 0x%016lx.\n", min_address, max_address);

    /* reset the ignored flags */
    pml4e = (pml4e_t *)phys2virt(pcr3->pml4_phys_4k * 4 * KB);
    for (uint32_t idx = 0 ; idx < 512 ; idx++, pml4e++)
    {
        pml4e->ignored = 0;
        if (!pml4e->present) continue;
        pdpte_t *pdpte = (pdpte_t *)phys2virt(pml4e->pdpt_phys_4k * 4 * KB);
        for (uint32_t jdx = 0 ; jdx < 512 ; jdx++, pdpte++)
        {
            pdpte->ignored = 0;
            if (!pdpte->present) continue;
            pde_t *pde = (pde_t *)phys2virt(pdpte->subt_phys_4k * 4 * KB);
            for (uint32_t kdx = 0 ; kdx < 512 ; kdx++, pde++)
            {
                pde->ignored = 0;
                if (!pde->present) continue;
                pte_t *pte = (pte_t *)phys2virt(pde->subt_phys_4k * 4 * KB);
                for (uint32_t ldx = 0 ; ldx < 512 ; ldx++, pte++)
                {
                    pte->ignored = 0;
                }
            }
        }
    }
    /*uint64_t *raw_pml4 = (uint64_t*)pml4;
    kprintk("pml4                 %p -- 0x%016x\n", pml4, *raw_pml4);
    kprintk("pml4->pte            0%04x\n", pml4->pte);
    kprintk("pml4->pde            0%04x\n", pml4->pde);
    kprintk("pml4->pdpte          0%04x\n", pml4->pdpte);*/
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
