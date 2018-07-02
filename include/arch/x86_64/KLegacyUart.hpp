#ifndef __KLEGACYUART_HPP__
#define __KLEGACYUART_HPP__

#include <stdint.h>

#define LEGACY_UART0_PORT   0x4
#define LEGACY_UART1_PORT   0x4

class KLegacyUart
{
private:
    uint8_t     uart_idx;
    uint32_t    interrupts_count;
protected:
    bool is_byte_available();
    bool is_write_done();

public:
	KLegacyUart(uint8_t uart_idx);
	~KLegacyUart();

	uint8_t readb();
    void writeb(uint8_t byte);

    uint8_t interrupt_identification_register;
    uint32_t get_interrupts_count() {return interrupts_count;}

    friend void _serial_callback(void);
};

#endif /* __KLEGACYUART_HPP__ */
