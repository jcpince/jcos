#include <arch/x86_64/KCpuHelper.hpp>

KCpuHelper *GetX86CpuHelper()
{
    static KCpuHelper *ch = (KCpuHelper*)NULL;
    if (ch == NULL) ch = new KCpuHelper();
    return ch;
}

KCpuHelper::KCpuHelper()
{

}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"

#define DECLARE_CRX_ACCESSORS_IMPL(crx)                     \
    uint64_t KCpuHelper::read_##crx()                       \
    {                                                       \
        register uint64_t result asm("rax");                \
        __asm__ __volatile__("mov  %" #crx ", %rax");       \
        return result;                                      \
    }                                                       \
    void KCpuHelper::write_##crx(uint64_t v)                \
    {                                                       \
        register uint64_t value asm("rax") = v;             \
        __asm__ __volatile__("mov  %rax, %" #crx);          \
    }                                                       \
    void KCpuHelper::set_##crx##_bit(x86_##crx##_bit_t bit) \
    {                                                       \
        register uint64_t crx asm("rax");                   \
        __asm__ __volatile__("mov  %" #crx ", %rax");       \
        SETBIT(crx, bit);                                   \
        __asm__ __volatile__("mov  %rax, %" #crx);          \
    }                                                       \
    void KCpuHelper::clr_##crx##_bit(x86_##crx##_bit_t bit) \
    {                                                       \
        register uint64_t crx asm("rax");                   \
        __asm__ __volatile__("mov  %" #crx ", %rax");       \
        CLRBIT(crx, bit);                                   \
        __asm__ __volatile__("mov  %rax, %" #crx);          \
    }

DECLARE_CRX_ACCESSORS_IMPL(cr0)
DECLARE_CRX_ACCESSORS_IMPL(cr2)
DECLARE_CRX_ACCESSORS_IMPL(cr3)
DECLARE_CRX_ACCESSORS_IMPL(cr4)
DECLARE_CRX_ACCESSORS_IMPL(cr8)

uint64_t KCpuHelper::read_msr(x86_msr_t msr)
{
    register uint64_t msr_idx asm("rcx") = msr;
    register uint64_t result asm("rax");
    asm("rdmsr");
    return result;
}

void KCpuHelper::write_msr(x86_msr_t msr, uint64_t value)
{
    register uint64_t msr_idx asm("rcx") = msr;
    register uint64_t result asm("rax") = value;
    asm("wrmsr");
}

void KCpuHelper::set_msr_bit(x86_msr_t msr, x86_msr_bit_t bit)
{
    register uint64_t msr_idx asm("rcx") = msr;
    register uint64_t msr_value asm("rax");
    asm("rdmsr");
    SETBIT(msr_value, bit);
    asm("wrmsr");
}

void KCpuHelper::clear_msr_bit(x86_msr_t msr, x86_msr_bit_t bit)
{
    register uint64_t msr_idx asm("rcx") = msr;
    register uint64_t msr_value asm("rax");
    asm("rdmsr");
    CLRBIT(msr_value, bit);
    asm("wrmsr");
}
#pragma GCC diagnostic pop
