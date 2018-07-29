#ifndef __KCPUHELPER__
#define __KCPUHELPER__

#include "x86_64_registers.hpp"

#include <stdint.h>
#include <stdlib.h>

#define SETBIT(register_, bit)      (register_ |= (1 << (bit)))
#define CLRBIT(register_, bit)      (register_ &= ~(1 << (bit)))
#define ISBITSET(value, bit)        (((value) & (1 << (bit))) != 0)
#define ISBITCLR(value, bit)        (((value) & (1 << (bit))) == 0)

#define DECLARE_CRX_ACCESSORS(crx)                  \
    uint64_t read_##crx();                          \
    void write_##crx(uint64_t value);               \
    void set_##crx##_bit(x86_##crx##_bit_t bit);    \
    void clr_##crx##_bit(x86_##crx##_bit_t bit);


class KCpuHelper
{
public:
    KCpuHelper();

    /* Configuration Registers */
    DECLARE_CRX_ACCESSORS(cr0)

    DECLARE_CRX_ACCESSORS(cr2)

    DECLARE_CRX_ACCESSORS(cr3)

    DECLARE_CRX_ACCESSORS(cr4)

    DECLARE_CRX_ACCESSORS(cr8)

    /* Model Specific Registers */
    uint64_t read_msr(x86_msr_t msr);

    void     write_msr(x86_msr_t msr, uint64_t value);

    void     set_msr_bit(x86_msr_t msr, x86_msr_bit_t bit);

    void     clear_msr_bit(x86_msr_t msr, x86_msr_bit_t bit);
private:
    ~KCpuHelper() {}
};

KCpuHelper *GetX86CpuHelper();

#endif /*__KCPUHELPER__ */
