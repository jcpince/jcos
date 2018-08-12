import gdb

VTABLE_SIZE             = 512
PHYS_PAGE_GRANULARITY   = 4*1024
virt_section_start      = 0
phys_section_start      = 0
section_size            = 0


def print_cr3():
    cr3 = int(gdb.parse_and_eval('&phys_pml4_start'))
    print("cr3 is 0x%x" % cr3)

def dump_pte(pml4e_index, pdpte_index, pde_index, pde_address, extraverbose=False):
    global virt_section_start, phys_section_start, section_size
    pte_address = int(gdb.parse_and_eval('((pde_t*)%d)->subt_phys_4k' % pde_address)) * PHYS_PAGE_GRANULARITY
    for idx in range(VTABLE_SIZE):
        value64 = int(gdb.parse_and_eval('*(uint64_t*)%d' % pte_address))
        if value64 != 0:
            phys_page_start = int(gdb.parse_and_eval('((pte_t*)%d)->page_phys_4k' % pte_address)) * PHYS_PAGE_GRANULARITY
            virt_page_start = (pml4e_index << 39) + (pdpte_index << 30) + (pde_index << 21) + (idx << 12)
            if extraverbose:
                print("pml4e_index(%d), pdpte_index(%d), pde_index(%d), pde_address(0x%x), pt[%03d -- 0x%016x] is 0x%016x -- %s" %
                     (pml4e_index, pdpte_index, pde_index, pde_address, idx, pte_address,
                     value64, str(gdb.parse_and_eval('*(pte_t*)%d' % pte_address))))
            if virt_section_start + section_size != virt_page_start:
                print("Physical memory 0x%016x--0x%016x mapped to 0x%016x--0x%016x" %
                    (phys_section_start, phys_section_start + section_size,
                        virt_section_start, virt_section_start+section_size))
                virt_section_start = virt_page_start
                phys_section_start = phys_page_start
                section_size = PHYS_PAGE_GRANULARITY
            else:
                section_size += PHYS_PAGE_GRANULARITY
        pte_address += 8

def dump_pde(pml4e_index, pdpte_index, pdpte_address, extraverbose=False):
    pde_address = int(gdb.parse_and_eval('((pdpte_t*)%d)->subt_phys_4k' % pdpte_address)) * PHYS_PAGE_GRANULARITY
    for idx in range(VTABLE_SIZE):
        value64 = int(gdb.parse_and_eval('*(uint64_t*)%d' % pde_address))
        if value64 != 0:
            if extraverbose:
                print("pd[%03d] is 0x%016x -- %s" %
                    (idx, value64, str(gdb.parse_and_eval('*(pde_t*)%d' % pde_address))))
            dump_pte(pml4e_index, pdpte_index, idx, pde_address, extraverbose)
        pde_address += 8

def dump_pdpte(pml4e_index, pml4e_address, extraverbose=False):
    pdpte_address = int(gdb.parse_and_eval('((pml4e_t*)%d)->pdpt_phys_4k' % pml4e_address)) * PHYS_PAGE_GRANULARITY
    for idx in range(VTABLE_SIZE):
        value64 = int(gdb.parse_and_eval('*(uint64_t*)%d' % pdpte_address))
        if value64 != 0:
            if extraverbose:
                print("pdpt[%03d] is 0x%016x -- %s" %
                    (idx, value64, str(gdb.parse_and_eval('*(pdpte_t*)%d' % pdpte_address))))
            dump_pde(pml4e_index, idx, pdpte_address, extraverbose)
        pdpte_address += 8


def dump_vtables(extraverbose=False):
    pml4 = int(gdb.parse_and_eval('&phys_pml4_start'))
    pml4e_address = pml4
    for idx in range(VTABLE_SIZE):
        value64 = int(gdb.parse_and_eval('*(uint64_t*)%d' % pml4e_address))
        if value64 != 0:
            if extraverbose:
                print("pml4[%03d] is 0x%016x -- %s" %
                    (idx, value64, str(gdb.parse_and_eval('*(pml4e_t*)%d' % pml4e_address))))
            dump_pdpte(idx, pml4e_address, extraverbose)
        pml4e_address += 8
    # print the last section
    print("Physical memory 0x%016x--0x%016x mapped to 0x%016x--0x%016x " %
        (phys_section_start, phys_section_start + section_size,
            virt_section_start, virt_section_start+section_size))
