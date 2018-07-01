#include "KInterruptManager.hpp"

#include <stdint.h>
#include <stdlib.h>
#include <asmf.H>

KInterruptManager::KInterruptManager()
{
	/* Insure the buffer is Page aligned */
	idtr.offset = vector_address = ((addr_t)idt_buffer + PAGESZ) & PAGEMASK;
	vector_len = sizeof(struct kidt_entry) * KIDT_NBVECTORS;
	idtr.limit = vector_len -1;
}

KInterruptManager::~KInterruptManager()
{
}

bool KInterruptManager::install_vector()
{
	__asm__("lidt (%0)":: "r"((uint64_t)&idtr));
	return true;
}

bool KInterruptManager::enable_interrupt(uint32_t interrupt __attribute__((__unused__)))
{
	return false;
}

bool KInterruptManager::disable_interrupt(uint32_t interrupt __attribute__((__unused__)))
{
	return false;
}

bool KInterruptManager::enable_irq(uint32_t irq_number __attribute__((__unused__)))
{
    if (irq_number > LEGACY_MAX_IRQ)
        return false;
    uint16_t imask = inportb(PIC0_PORTB_NUMBER);
    outportb(PIC0_PORTB_NUMBER, imask & ~(1 << irq_number));
    return true;
}

bool KInterruptManager::disable_irq(uint32_t irq_number __attribute__((__unused__)))
{
    if (irq_number > LEGACY_MAX_IRQ)
        return false;
    uint16_t imask = inportb(PIC0_PORTB_NUMBER);
    outportb(PIC0_PORTB_NUMBER, imask | (1 << irq_number));
    return true;
}

bool KInterruptManager::add_interrupt_handler(uint32_t interrupt, kinterrupt_handler_t handler)
{
	if (interrupt >= KIDT_NBVECTORS) return false;

	struct kidt_entry *entry = &((struct kidt_entry *)vector_address)[interrupt];
	addr_t handler_offset = (addr_t)handler;
	entry->offset_low = handler_offset & 0xffff;
	entry->offset_high = (handler_offset>>16) & 0xffffffffffff;
	entry->segment_selector = KSEG_CS; /* Should be given by the memory manager */
	entry->ist = KIDT_ENTRY_IST;
	entry->type = KIDT_ENTRY_TYPE_IG64;
	entry->priviledge = 0;
	entry->presence = 1;
	entry->zero_low = 0;
	entry->zero_high = 0;
	entry->reserved = 0;
	return true;
}

#include <stdio.h>

bool KInterruptManager::add_irq_handler(uint32_t irq_number, kinterrupt_handler_t handler)
{
	uint32_t interrupt = irq2interrupt(irq_number);
    if (interrupt == INVALID_INTERRUPT)
        return false;

    uint32_t flags = savei();
    disablei();
    add_interrupt_handler(interrupt, handler);
    enable_irq(irq_number);
	restaurei(flags);
	return true;
}

bool KInterruptManager::remove_interrupt_handler(uint32_t interrupt __attribute__((__unused__)),
	kinterrupt_handler_t handler __attribute__((__unused__)))
{
	struct kidt_entry *entry = &((struct kidt_entry *)vector_address)[interrupt];
	entry->presence = 0;
	return true;
}

bool KInterruptManager::remove_irq_handler(uint32_t irq_number, kinterrupt_handler_t handler)
{
	uint32_t interrupt = irq2interrupt(irq_number);
    if (interrupt == INVALID_INTERRUPT)
        return false;

    bool result;
    uint32_t flags = savei();
    result = remove_interrupt_handler(interrupt, handler);
    if (result)
        disable_irq(irq_number);
    restaurei(flags);
    return result;
}
