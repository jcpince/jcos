#include <drivers/serial/KSerial.hpp>

#include <asmf.H>

static constexpr uint16_t uart_ports[4] = { 0x3f8, 0x2f8, 0x3e8, 0x2e8 };

KSerial::KSerial(uint16_t portid)
{
    this->portid = portid;
    outportb(uart_ports[portid] + 1, 0x0f);    // Disable all interrupts
    outportb(uart_ports[portid] + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    outportb(uart_ports[portid] + 0, 0x01);    // Set divisor to 1 (lo byte) 115200 baud
    outportb(uart_ports[portid] + 1, 0x00);    //                  (hi byte)
    outportb(uart_ports[portid] + 3, 0x03);    // 8 bits, no parity, one stop bit
    outportb(uart_ports[portid] + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    //outportb(uart_ports[portid] + 1, 0x0f);    // Enable all interrupts
    outportb(uart_ports[portid] + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

KSerial::~KSerial()
{
}

bool KSerial::is_byte_available()
{
    return inportb(uart_ports[portid] + 5) & 1;
}

bool KSerial::is_write_done()
{
    return inportb(uart_ports[portid] + 5) & 0x20;
}

uint8_t KSerial::readb()
{
    while (is_byte_available() == 0);
    return inportb(uart_ports[portid]);
}

void KSerial::writeb(uint8_t byte)
{
   while (is_write_done() == 0);
   outportb(uart_ports[portid], byte);
}
