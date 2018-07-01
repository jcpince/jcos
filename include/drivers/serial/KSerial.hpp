#ifndef __KSERIAL_HPP__
#define __KSERIAL_HPP__

#include <stdint.h>

class KSerial
{
private:
    uint16_t portid;
protected:
    bool is_byte_available();
    bool is_write_done();

public:
	KSerial(uint16_t portid);
	~KSerial();

	uint8_t readb();
    void writeb(uint8_t byte);
};

#endif /* __KSERIAL_HPP__ */
