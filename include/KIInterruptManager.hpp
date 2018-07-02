#ifndef __KIINTERRUPTMANAGER_HPP__
#define __KIINTERRUPTMANAGER_HPP__

#include <stdint.h>

#define GET_ISR_NAME(name) isr_##name
#define DECLARE_ISR(name)               \
    extern "C" void isr_##name(void);   \
    extern "C" void callback_##name(void)

typedef void (*kinterrupt_handler_t) (void);

/*
	TODO: Inherits from a singleton design pattern
*/
class KIInterruptManager
{
protected:
	KIInterruptManager()
	{
		/* set default fad values so that multiple remapping can be trapped */
		vector_address = -1;
		vector_len = -1;
	}
	~KIInterruptManager() {}

	addr_t vector_address;
	uint32_t vector_len;

public:
	virtual uint32_t savei() = 0;
	virtual void restaurei(uint32_t flags) = 0;
	virtual void enablei() = 0;
	virtual void disablei() = 0;
	virtual void enablenmi() = 0;
	virtual void disablenmi() = 0;
	virtual void enableall() = 0;
	virtual void disableall() = 0;
	virtual bool install_vector() = 0;
	virtual bool enable_irq(uint32_t irq_number) = 0;
	virtual bool disable_irq(uint32_t irq_number) = 0;
	virtual bool enable_interrupt(uint32_t interrupt) = 0;
	virtual bool disable_interrupt(uint32_t interrupt) = 0;
	virtual bool generate_interrupt(uint32_t interrupt) = 0;
	virtual bool add_irq_handler(uint32_t irq_number, kinterrupt_handler_t handler) = 0;
	virtual bool add_interrupt_handler(uint32_t interrupt, kinterrupt_handler_t handler) = 0;
	virtual bool remove_interrupt_handler(uint32_t interrupt, kinterrupt_handler_t handler) = 0;
	virtual bool remove_irq_handler(uint32_t irq_number, kinterrupt_handler_t handler) = 0;
    virtual uint32_t irq2interrupt(uint32_t irq_number) = 0;
};

KIInterruptManager *GetInterruptManager();

#endif /* __KIINTERRUPTMANAGER_HPP__ */
