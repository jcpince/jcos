#ifndef __KINTERRUPTMANAGER_HPP__
#define __KINTERRUPTMANAGER_HPP__

#include <KIInterruptManager.hpp>

/* 
	TODO: Include this from a unique header
*/
#define KSEG_CS		0x8
#define KSEG_DS		0x8
#define PAGESZ		0x1000
#define PAGEMASK	(~(PAGESZ-1))

#define KIDT_ENTRY_IST		0x0	/* default value */

#define KIDT_ENTRY_TYPE_LDT	0x2	/* LDT */
#define KIDT_ENTRY_TYPE_TSS64A	0x9	/* 64-bit TSS (Available) */
#define KIDT_ENTRY_TYPE_TSS64B	0xB	/* 64-bit TSS (Busy) */
#define KIDT_ENTRY_TYPE_CG64	0xC	/* 64-bit Call Gate */
#define KIDT_ENTRY_TYPE_IG64	0xE	/* 64-bit Interrupt Gate */
#define KIDT_ENTRY_TYPE_TG64	0xF	/* 64-bit Trap Gate */

#define KIDT_NBVECTORS		256

struct kidt_entry {
	uint16_t	offset_low;
	uint16_t	segment_selector;
	uint8_t		ist : 3;
	uint8_t		zero_low : 5;
	uint8_t		type : 4;
	uint8_t		zero_high : 1;
	uint8_t		priviledge : 2;
	uint8_t		presence : 1;
	uint64_t	offset_high : 48;
	uint32_t	reserved;
} __attribute__((packed));

struct kidtr {
	uint16_t	limit;
	uint64_t	offset;
} __attribute__((packed));

class KInterruptManager : public KIInterruptManager
{
private:
	kidtr idtr;
	char *idt_buffer[sizeof(struct kidt_entry) * KIDT_NBVECTORS + PAGESZ];

public:
	KInterruptManager();
	~KInterruptManager();
	
	void enablei() {__asm__("sti");}
	void disablei() {__asm__("cli");}
	uint32_t savei() {uint64_t flags;__asm__ __volatile__("pushf; pop %0": "=rm"(flags));return (flags)&0xffffffff;}
	void restaurei(uint32_t flags) {__asm__ __volatile__("push %0; popf":: "g"(flags));}
	void enablenmi() {__asm__ __volatile__("in $0x70, %al;and $0x80, %al;out %al, $0x70");}
	void disablenmi() {__asm__ __volatile__("in $0x70, %al;and $0x7f, %al;out %al, $0x70");}
	void enableall() {enablei();enablenmi();}
	void disableall() {disablei();disablenmi();}
	bool generate_interrupt(uint32_t interrupt)
	{
		switch (interrupt)
		{
			case 0: __asm__("int $0");break;
			case 1: __asm__("int $1");break;
			case 2: __asm__("int $2");break;
			case 3: __asm__("int $3");break;
			case 4: __asm__("int $4");break;
			case 5: __asm__("int $5");break;
			case 6: __asm__("int $6");break;
			case 7: __asm__("int $7");break;
			default: return false;
		}
		return true;
	}
	bool install_vector();
	bool enable_interrupt(uint32_t interrupt);
	bool disable_interrupt(uint32_t interrupt);
	bool add_handler(uint32_t interrupt, kinterrupt_handler_t handler);
	bool remove_handler(uint32_t interrupt, kinterrupt_handler_t handler);	
};

KIInterruptManager *kim = new KInterruptManager();

#endif /* __KINTERRUPTMANAGER_HPP__ */
