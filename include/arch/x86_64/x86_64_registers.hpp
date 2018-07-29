#ifndef __X86_64_MSRS__
#define __X86_64_MSRS__

typedef enum
{
    X86_CR0_PE = 0, /* Protected Mode Enable */
    X86_CR0_PG = 31, /* Paging */
    /* ... */
} x86_cr0_bit_t;

typedef enum
{
    X86_CR2_PF_BIT0,
    /* PAGE FAULT ADDRESS */
} x86_cr2_bit_t;

typedef enum
{
    X86_CR3_PWT = 3,
    /* ... */
} x86_cr3_bit_t;

typedef enum
{
    X86_CR4_VME   = 0,
    X86_CR4_PAE   = 5,  /* Physical Address Extension */
    X86_CR4_PCIDE = 17, /* PCID Enable */
    /* ... */
} x86_cr4_bit_t;

typedef enum
{
    X86_CR8_PRIORITY_BIT0 = 0,
    X86_CR8_PRIORITY_BIT1 = 1,
    X86_CR8_PRIORITY_BIT2 = 2,
    X86_CR8_PRIORITY_BIT3 = 3,
} x86_cr8_bit_t;

typedef enum
{
    X86_64_MSR_EFER = 0xC0000080,
} x86_msr_t;

typedef enum
{
    /* X86_64_MSR_EFER bits */
    X86_64_MSR_EFER_SCE   = 0,  /* System Call Extensions */
    X86_64_MSR_EFER_LME   = 8,  /* Long Mode Enable */
    X86_64_MSR_EFER_LMA   = 10, /* Long Mode Active */
    X86_64_MSR_EFER_NXE   = 11, /* No-Execute Enable */
    X86_64_MSR_EFER_SVME  = 12, /* Secure Virtual Machine Enable */
    X86_64_MSR_EFER_LMSLE = 13, /* Long Mode Segment Limit Enable */
    X86_64_MSR_EFER_FFXSR = 14, /* Fast FXSAVE/FXRSTOR */
    X86_64_MSR_EFER_TCE   = 15, /* Translation Cache Extension */
} x86_msr_bit_t;

#endif /* __X86_64_MSRS__ */
