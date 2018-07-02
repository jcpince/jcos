#include <arch/x86_64/KLegacyUart.hpp>
#include <KIInterruptManager.hpp>

#include "KInterruptManager.hpp"

#include <stdlib.h>
#include <asmf.H>

static constexpr uint16_t uart_ports[4] = { 0x3f8, 0x2f8, 0x3e8, 0x2e8 };
static KLegacyUart *port0 = (KLegacyUart *)NULL;

void _serial_callback(void)
{
    if (!port0) return;

    if (port0->is_byte_available())
    {
        uint8_t b = port0->readb();
        port0->writeb(b);
    }
    outportb(PIC0_PORTA_NUMBER, PIC_INT_ACK);
}

DECLARE_ISR(serial)
{
    _serial_callback();
}


KLegacyUart::KLegacyUart(uint8_t uart_idx)
{
    this->uart_idx = uart_idx;

    outportb(uart_ports[uart_idx] + 1, 0x01);    // Enable only Rx
    outportb(uart_ports[uart_idx] + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    outportb(uart_ports[uart_idx] + 0, 0x01);    // Set divisor to 1 (lo byte) 115200 baud
    outportb(uart_ports[uart_idx] + 1, 0x00);    //                  (hi byte)
    outportb(uart_ports[uart_idx] + 3, 0x03);    // 8 bits, no parity, one stop bit
    outportb(uart_ports[uart_idx] + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outportb(uart_ports[uart_idx] + 4, 0x0B);    // IRQs enabled, RTS/DSR set

    KIInterruptManager *kim = GetInterruptManager();
    kim->add_irq_handler(4, GET_ISR_NAME(serial));
    port0 = this;
}

KLegacyUart::~KLegacyUart()
{
}

bool KLegacyUart::is_byte_available()
{
    return inportb(uart_ports[uart_idx] + 5) & 1;
}

bool KLegacyUart::is_write_done()
{
    return inportb(uart_ports[uart_idx] + 5) & 0x20;
}

uint8_t KLegacyUart::readb()
{
    while (is_byte_available() == 0);
    return inportb(uart_ports[uart_idx]);
}

void KLegacyUart::writeb(uint8_t byte)
{
   while (is_write_done() == 0);
   outportb(uart_ports[uart_idx], byte);
}
