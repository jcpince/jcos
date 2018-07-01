#include "KInterruptManager.hpp"

#include <stdint.h>
#include <stdlib.h>

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

bool KInterruptManager::add_handler(uint32_t interrupt, kinterrupt_handler_t handler)
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

bool KInterruptManager::remove_handler(uint32_t interrupt __attribute__((__unused__)),
	kinterrupt_handler_t handler __attribute__((__unused__)))
{
	struct kidt_entry *entry = &((struct kidt_entry *)vector_address)[interrupt];
	entry->presence = 0;
	return true;
}
