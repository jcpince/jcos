#ifndef __ASM_DEFS_H__
#define __ASM_DEFS_H__

#include <arch/x86_64/system.h>

/* setting up the Multiboot2 header - see GRUB docs for details */
#define MB2_ALIGN               (1<<0)                      /* align loaded modules on page boundaries */
#define MB2_MEMINFO             (1<<1)                      /* provide memory map */
#define MB2_FLAGS               (MB2_ALIGN | MB2_MEMINFO)   /* this is the Multiboot2 'flag' field */
#define MB2_HDR_MAGIC           0xE85250D6                  /* this is the Multiboot2 header 'magic' field */
#define MB2_BTL_MAGIC           0x36D76289                  /* this is the Multiboot2 bootloader 'magic' field */
#define MB2_ARCH                0                           /* this is the Multiboot2 'arch' field */
#define MB2_HEADERLEN           16                          /* this is the Multiboot2 'header_length' field */
#define MB2_CHECKSUM            \
  (-(MB2_HDR_MAGIC + MB2_ARCH + MB2_HEADERLEN) & 0xffffffff)/* this is the Multiboot2 'checksum' field */

#define EFLAGS_IDBIT            21
#define CPUID_HIGHESTFUNC       0x80000000
#define CPUID_PROCINFO          0x80000001
#define CPUID_PROCINFO_LM_BIT   29

#define PAGE_PRESENT            (1 << 0)
#define PAGE_WRITE              (1 << 1)

#define CODE_SEG                0x0008
#define DATA_SEG                0x0010

#define VM_TABLE_ENTRIES        512
#define VM_TABLE_ENTRY_SIZE     8
#define VM_TABLE_SIZE           (VM_TABLE_ENTRIES * VM_TABLE_ENTRY_SIZE)
#define PML4_BIT0_OFF           39
#define PML4_TABLE_SIZE         VM_TABLE_SIZE
#define PDPT_TABLE_SIZE         VM_TABLE_SIZE
#define PDE_TABLE_SIZE          VM_TABLE_SIZE
#define PTE_TABLE_SIZE          (VM_TABLE_ENTRIES * VM_TABLE_ENTRY_SIZE)
#define PML4_MASK               0x1FF
#define PDPT_BIT0_OFF           30
#define PDPT_MASK               0x1FF

#define PML4_TABLE_OFFSET       phys_pml4_start
#define PDPT_TABLE_OFFSET       (phys_pml4_start + VM_TABLE_SIZE)
#define PDE_TABLE_OFFSET        (PDPT_TABLE_OFFSET + VM_TABLE_SIZE)
#define PTE_NB_TABLES           (16)
#define PTE_TABLE0_OFFSET       (PDE_TABLE_OFFSET + VM_TABLE_SIZE)
#define PTE_TABLE1_OFFSET       (PTE_TABLE0_OFFSET + VM_TABLE_SIZE)
#define PTE_TABLE2_OFFSET       (PTE_TABLE1_OFFSET + VM_TABLE_SIZE)
#define PTE_TABLE3_OFFSET       (PTE_TABLE2_OFFSET + VM_TABLE_SIZE)
#define PTE_TABLE4_OFFSET       (PTE_TABLE3_OFFSET + VM_TABLE_SIZE)
#define PTE_TABLE5_OFFSET       (PTE_TABLE4_OFFSET + VM_TABLE_SIZE)
#define PTE_TABLE6_OFFSET       (PTE_TABLE5_OFFSET + VM_TABLE_SIZE)
#define PTE_TABLE7_OFFSET       (PTE_TABLE6_OFFSET + VM_TABLE_SIZE)
#define PTE_TABLE8_OFFSET       (PTE_TABLE7_OFFSET + VM_TABLE_SIZE)
#define PTE_TABLE9_OFFSET       (PTE_TABLE8_OFFSET + VM_TABLE_SIZE)
#define PTE_TABLE10_OFFSET      (PTE_TABLE9_OFFSET + VM_TABLE_SIZE)
#define PTE_TABLE11_OFFSET      (PTE_TABLE10_OFFSET + VM_TABLE_SIZE)
#define PTE_TABLE12_OFFSET      (PTE_TABLE11_OFFSET + VM_TABLE_SIZE)
#define PTE_TABLE13_OFFSET      (PTE_TABLE12_OFFSET + VM_TABLE_SIZE)
#define PTE_TABLE14_OFFSET      (PTE_TABLE13_OFFSET + VM_TABLE_SIZE)
#define PTE_TABLE15_OFFSET      (PTE_TABLE14_OFFSET + VM_TABLE_SIZE)

#endif /* __ASM_DEFS_H__ */
