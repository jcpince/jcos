#include <init/KMultibootManagerFactory.hpp>
#include <arch/x86_64/KLegacyUart.hpp>
#include <kernel/KMemoryManager.hpp>
#include <KIInterruptManager.hpp>
#include <KOStream.hpp>
#include <MyClass.hpp>

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>

int dbg = 1;

extern "C" void page_fault_isr();

#define IO_KBD0_PORT        (0x60)
#define PIC0_PORTA_NUMBER   (0x20)
#define PIC_INT_ACK         (0x20)
#define KEYBOARD_IRQ_NUMBER (0x1)

extern uint64_t _start;
extern uint64_t _heap_start;

DECLARE_ISR(keyboard)
{
	uint8_t key, scancode;
	bool	keypressed;
    key = inportb(IO_KBD0_PORT);
    outportb(PIC0_PORTA_NUMBER, PIC_INT_ACK);
	keypressed = (key & 0x80) == 0;
	scancode = key & 0x7f;
	printk("Keyboard interrupt: got 0x%x %s: code 0x%x '%c'\n", key&0xff, (keypressed ? "pressed" : "released"), scancode, scancode);
}

DECLARE_ISR(int0)
{
	printk("Interrupt 0 caught\n");
}

DECLARE_ISR(int1)
{
	printk("Interrupt 1 caught\n");
}

DECLARE_ISR(int2)
{
	printk("Interrupt 2 caught\n");
}

DECLARE_ISR(int3)
{
	printk("Interrupt 3 caught\n");
}

extern "C" void page_fault_callback(uint64_t bad_address, uint64_t fault_pc, uint64_t pfflags)
{
	printk("Page fault caught with at the access 0x%016lx, return@ is 0x%016lx - flags(0x%016lx)\n",
		bad_address, fault_pc, pfflags);
	switch (pfflags)
	{
		case 0: printk("Supervisor process tried to read a non-present page entry\n");break;
		case 1: printk("Supervisor process tried to read a page and caused a protection fault\n");break;
		case 2: printk("Supervisor process tried to write to a non-present page entry\n");break;
		case 3: printk("Supervisor process tried to write a page and caused a protection fault\n");break;
		case 4: printk("User process tried to read a non-present page entry\n");break;
		case 5: printk("User process tried to read a page and caused a protection fault\n");break;
		case 6: printk("User process tried to write to a non-present page entry\n");break;
		case 7: printk("User process tried to write a page and caused a protection fault\n");break;
		default: printk("Bad page fault flags\n");
	}
	while(1);
}

extern "C" void kmain(uint32_t mbi, uint32_t bootloader_magic)
{
    /* Shall be called early on to allow interrupts */
    KIInterruptManager *kim = GetInterruptManager();
    kim->install_vector();

	Myclass c;
	uint64_t count = 0;
    KLegacyUart ttyS0(0);

	kstd::kout.SetTextColour(BLACK);
	kstd::kout.SetBackColour(WHITE);
	kstd::kout.clear();

	kstd::kout.printf("sizeof(unsigned long): %d\n", sizeof(unsigned long));

	//while(dbg);

	KIMultibootManager *mbmgr = kmmf.getMultibootMgr(bootloader_magic, (addr_t)mbi);
	if (!mbmgr)
	{
		kstd::kout.printf("Was not able to get a multi-boot manager with magic 0x%x\n", bootloader_magic);
	}
	else
	{
		kusable_memory_region_t *regions = mbmgr->getMemoryRegions();
        kusable_memory_region_t *region = &regions[0];
		while (region->size != 0)
		{
			kstd::kout.printf("Found a region from 0x%lx to 0x%lx -- size ",
                    region->base_address, region->base_address + region->size);
			if (region->size > GB) {
				kstd::kout.printf("%d GiB\n", region->size/GB);
			}
			else if (region->size > MB) {
				kstd::kout.printf("%d MiB\n", region->size/MB);
			}
			else if (region->size > KB) {
				kstd::kout.printf("%d KiB\n", region->size/KB);
			}
			else {
				kstd::kout.printf("%d Bytes\n", region->size);
			}
			region++;
		}
		char *bootloadername = mbmgr->getBootloaderName();
		char *commandline = mbmgr->getCommandLine();

		kstd::kout << _L("Bootloader name: ") << bootloadername << kstd::endl;
		kstd::kout << _L("Command line: ") << commandline << kstd::endl;

		free(bootloadername);
		free(commandline);
		free(regions);
		delete mbmgr;
	}

    uint32_t *allocated_data = (uint32_t *)malloc(100*sizeof(uint32_t));
    kstd::kout.printf("allocated_data: %p\n", allocated_data);
    allocated_data[0] = 20;
    kstd::kout.printf("allocated_data[0]: %u\n", allocated_data[0]);

	try
	{
		c.test_throw();
	}
	catch(int e)
	{
		kstd::kout << count++ << _L(": Integer exception (") << e << _L(") caught!!!") << kstd::endl;
	}
	catch(...)
	{
		kstd::kout << count++ << _L(": Unexpected exception caught!!!") << kstd::endl;
	}

	kim->add_interrupt_handler(0, GET_ISR_NAME(int0));
	kim->add_interrupt_handler(1, GET_ISR_NAME(int1));
	kim->add_interrupt_handler(2, GET_ISR_NAME(int2));
	kim->add_interrupt_handler(3, GET_ISR_NAME(int3));
	kim->add_interrupt_handler(14, page_fault_isr);
    kim->add_irq_handler(KEYBOARD_IRQ_NUMBER, GET_ISR_NAME(keyboard));
	kim->enableall();

	kstd::kout << _L("done, generates interrupt 0") << kstd::endl;
	kim->generate_interrupt(0);
	kstd::kout << _L("done, generates interrupt 2") << kstd::endl;
	kim->generate_interrupt(2);

	kstd::kout << _L("That's all folks!!") << kstd::endl;

    /* Go to sleep mode... */
    kstd::kout.printf("&_start      0x%lx\n", &_start);
    kstd::kout.printf("&_heap_start 0x%lx\n", &_heap_start);
    uint32_t stack_int;
    kstd::kout.printf("&stack_int   0x%lx\n", &stack_int);
    kstd::kout.printf("kim          %p\n", kim);
    while(1)
    {
        printk("Entering sleep...\n");
        __asm__("hlt");
        printk("Woken\n");
    }

	/* Test the crash handler
    int *ptr = (int *)0xFFFFFFFFA0000000;
	*ptr = 7;
	int test = *ptr;
	kstd::kout << _L("That's all folks!!") << test << kstd::endl;*/
}
