.code32
.section .bootstrap

#include "asm_defs.h"

#define clear_vtable(table_offset)  \
    mov $(table_offset), %edi;      \
    push %edi;                      \
    push %eax;                      \
    push %ecx;                      \
    mov $(4*KB), %ecx;              \
    xor %eax, %eax;                 \
    rep stosl;                      \
    pop %edi;                       \
    pop %eax;                       \
    pop %ecx;

/* Multiboot2 header */
MULTIBOOT2_HEADER:
    .align 4
    .long MB2_HDR_MAGIC
    .long MB2_ARCH
    .long MB2_HEADERLEN
    .long MB2_CHECKSUM
    .long 0
    .long 0

/* Stack buffer */
.comm stack, STACKSIZE, 32                  /* reserve 16k stack on a quadword boundary */

/* 32-bit entry point */
.globl loader32
loader32:
#if (EARLYDBG == 1)
1:  jmp 1b
#endif

    mov $(stack_phys + STACKSIZE), %esp     /* set up the stack on top of the buffer reserved for it */
    push  %eax                              /* Multiboot magic number */
    push  %ebx                              /* Multiboot data structure address */

#if (EARLYDBG == 2)
1:  jmp 1b
#endif

    call asm_check_cpuid
    jc cpuid_not_supported

    call asm_check_longmode
    jc lm_not_supported

    /* Will never return from long mode */
    jmp activate_long_mode

    /* But just in case... */
    jmp lm_not_supported

asm_check_cpuid:
    pushf                                   /* push the EFLAGS in the stack */
    pop %eax                                /* Get the EFLAGS into eax */
    mov %eax, %ecx                          /* Backup original EFLAGS into ecx */
    xor $(1 << EFLAGS_IDBIT), %eax          /* Invert the bit ID in EFLAGS */
    push %eax
    popf                                    /* Restaure the new EFLAGS */

    pushf
    pop %eax
    xor %ecx, %eax                          /* Verify that ID-bit changed */
    shr $EFLAGS_IDBIT, %eax                 /* Shift the ID-bit so it becomes the bit 0 */
    and $1, %eax                            /* Check if the bit 0 is set */
    push %ecx                               /* restaure original EFLAGS */
    popf

    test %eax, %eax                        /* test the bit 0 */
    je .bad_cpu
    ret
.bad_cpu:
    stc
    ret

asm_check_longmode:
    /* Call CPUID so that we check that the PROCINFO function is supported by CPUID */
    mov $CPUID_HIGHESTFUNC, %eax
    cpuid

    /* Compare the highest function with PROCINFO */
    cmp $CPUID_PROCINFO, %eax
    jb .no_procinf

    /* Call CPUID with PROCINFO */
    mov $CPUID_PROCINFO, %eax
    cpuid
    test $(1 << CPUID_PROCINFO_LM_BIT), %edx
    jz .no_lm
    ret
.no_lm:
.no_procinf:
    stc
    ret

cpuid_not_supported:
    jmp cpuid_not_supported

lm_not_supported:
    jmp lm_not_supported

activate_long_mode:
    /* 32-bit to 64-bit code */
    /* 1. Have paging disabled */
    mov %cr0, %eax                          /* Deactivate long mode */
    and $0x7fffffff, %eax                   /* by disabling paging. */
    mov %eax, %cr0

    /* Put EDI on the buffer reserved for PML4/PDP/PD and PT */


#if (EARLYDBG == 3)
1:	jmp 1b
#endif
    /*
      Maps 16MB phys addresses from 0x0--0x1000000 to 0x80000000--0x81000000
        PML4 entry 0
        PDPTE entry 2
        PDE entries 0 to PTE_NB_TABLES-1
        PTE_NB_TABLES PTE tables all entries counting up

        VM tables are all 4KB aligned so their address can be put directly into
        the tables.
     */

    /* Publish the PML4 address */
    mov $(PML4_TABLE_OFFSET), %edi
    mov %edi, %cr3                          /* Point CR3 at the PML4. */

    /* Build Page Map Level 4 */
    clear_vtable(PML4_TABLE_OFFSET)
    mov $(PML4_TABLE_OFFSET), %edi
    lea PML4_TABLE_SIZE(%edi), %eax         /* Put the address of the Page Directory Pointer Table in to EAX */
    or $(PAGE_PRESENT | PAGE_WRITE), %eax   /* Or EAX with the flags - present flag, writable flag */
    mov %eax, %es:((VM_TABLE_ENTRIES-1)*8)(%edi)/* Store the value of EAX as the first PML4 entry */
    mov %eax, %es:0x0(%edi)                   /* Store the value of EAX as the first PML4 entry */

    /* Build the Page Directory Pointer */
    clear_vtable(PDPT_TABLE_OFFSET)
    mov $(PDPT_TABLE_OFFSET), %edi
    lea PDPT_TABLE_SIZE(%edi), %eax         /* Put the address of the Page Directory in to EAX */
    or $(PAGE_PRESENT | PAGE_WRITE), %eax   /* Or EAX with the flags - present flag, writable flag */
    /* Mapping of the virtual addresses from 0 to the physical memory 0 */
    /* Sets int index 510 of PDPT for virtual addresses from 0xffffffff80000000 */
    /* NOTE: since we're using the same tables, enabling both entries in PML4 and PDPTE will
     * create mappings for 0x0, 0xffffffff80000000 but also 0x7f80000000 and 0x0000ff8000000000...
     */
    mov %eax, %es:((VM_TABLE_ENTRIES-2)*8)(%edi)/* Store the value of EAX as the third PDPT entry */
    mov %eax, %es:0x0(%edi)                 /* Store the value of EAX as the third PDPT entry */

    /* Build the Page Directory */
    clear_vtable(PDE_TABLE_OFFSET)
    mov $(PDE_TABLE_OFFSET), %edi
    lea PDE_TABLE_SIZE(%edi), %eax          /* Put the address of the first Page Table in to EAX */
    or $(PAGE_PRESENT | PAGE_WRITE), %eax   /* Or EAX with the flags - present flag, writable flag */
    mov $(PTE_NB_TABLES), %ecx
    mov $(0), %ebx
set_pdes:
    mov %eax, %es:0x0(%edi)
    mov %ebx, %es:0x4(%edi)
    add $(PTE_TABLE_SIZE), %eax
    add $(VM_TABLE_ENTRY_SIZE), %edi
    dec %ecx
    cmp %ecx, %ebx
    jne set_pdes

#if (EARLYDBG == 4)
1:	jmp 1b
#endif

    /* Build the page tables */
    mov $(PTE_TABLE0_OFFSET), %edi
    mov $(PAGE_PRESENT | PAGE_WRITE), %eax
    mov $(0), %ebx
    mov $(PTE_TABLE0_OFFSET + PTE_NB_TABLES * PTE_TABLE_SIZE + PTE_TABLE_SIZE), %ecx
set_ptes:
    mov %eax, %es:(%edi)
    mov %ebx, %es:0x4(%edi)
    add $(PAGESZ), %eax
    add $(VM_TABLE_ENTRY_SIZE), %edi
    cmp %ecx, %edi
    jne set_ptes

#if (EARLYDBG == 5)
1:	jmp 1b
#endif

    /* Disable PIC 16 interrupts */
    mov $0xff, %al
    out %al, $0xA1
    mov $0xff, %al
    out %al, $0x21

    /* 2. Set the PAE enable bit in CR4 */
    /* Enter long mode. */
    mov %cr4, %eax
    or $0xA0, %eax                             /* Set the PAE and PGE bits.*/
    mov %eax, %cr4

    /* 4. Enable long mode by setting the EFER.LME flag in MSR 0xC0000080	*/
    mov $0xC0000080, %ecx
    rdmsr
    or $0x00000100, %eax
    wrmsr                                      /* Set the LME bit */

    /* 5. Enable paging */
    mov %cr0, %eax                             /* Activate long mode */
    or $0x80000000, %eax                       /* by enabling paging */
    mov %eax, %cr0

    /* Jump to 64-bits mode with a long jump (or lret) */
    lgdt GDT64.Pointer
    ljmpl $0x0008, $LongMode

.align 4
GDT64:
GDT64.NULL:
    .quad 0x0
GDT64.64bits.kcode:
    .quad 0x00AF9A000000FFFF
GDT64.64bits.kdata:
    .quad 0x00CF92000000FFFF
GDT64.64bits.kstack:
    .quad 0x00CF92000000FFFF
GDT64.64bits.ucode:
    .quad 0x00CFFA000000FFFF
GDT64.64bits.udata:
    .quad 0x00CFF2000000FFFF
.align 4
    .word 0
GDT64.Pointer:
    .word GDT64.Pointer - GDT64 - 1            /* lenght on 16 bits */
    .quad GDT64                                /* pointer to the data on 64 bits */

.code64
.align 4
LongMode:
    /* We are now in LongMode,
      we can setup the segments and call the
      kernel main */
    mov $0x0010, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov $0x0018, %ax
    mov %ax, %ss

    /*1:  jmp 1b*/

    /* jmp and not push so that the stack is not modified */
    movabsq $kernel_start64, %rax
    jmp *%rax

    /* Should never arrive here but just in case */
    cli
hang:
    hlt                                     /* halt machine should kernel return */
    jmp   hang
