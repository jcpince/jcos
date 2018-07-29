#include <init/KMultibootManagerFactory.hpp>
#include <mem/KIVirtualMemoryManager.hpp>
#include <arch/x86_64/KLegacyUart.hpp>
#include <arch/x86_64/KCpuHelper.hpp>
#include <kernel/KMemoryManager.hpp>
#include <KIInterruptManager.hpp>
#include <KOStream.hpp>
#include <MyClass.hpp>

#include <mem/dlmalloc.h>
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

static void show_malloc_stats(KLegacyUart &uart)
{
    struct mallinfo mi = mallinfo();
    uart.printf("******************************************\n");
    uart.printf("Malloc stats:\n");
    uart.printf("Non-mmapped space allocated (bytes):           0x%08x\n", mi.arena);
    uart.printf("Number of free chunks:                         0x%08x\n", mi.ordblks);
    uart.printf("Number of free fastbin blocks:                 0x%08x\n", mi.smblks);
    uart.printf("Number of mmapped regions:                     0x%08x\n", mi.hblks);
    uart.printf("Space allocated in mmapped regions (bytes):    0x%08x\n", mi.hblkhd);
    uart.printf("Maximum total allocated space (bytes):         0x%08x\n", mi.usmblks);
    uart.printf("Space in freed fastbin blocks (bytes):         0x%08x\n", mi.fsmblks);
    uart.printf("Total allocated space (bytes):                 0x%08x\n", mi.fordblks);
    uart.printf("Top-most, releasable space (bytes):            0x%08x\n", mi.keepcost);
    uart.printf("******************************************\n");
}

KLegacyUart *kernel_console = (KLegacyUart *)NULL;

extern "C" void kprintk (const char *fmt, ...)
{
    va_list args;
	char printbuffer[CFG_PBSIZE];

	va_start (args, fmt);

	/* For this to work, printbuffer must be larger than
	 * anything we ever want to print.
	 */
	vsprintf (printbuffer, fmt, args);
	va_end (args);
    if (kernel_console)
        kernel_console->writes(printbuffer);
    kstd::kout << printbuffer;
}

extern "C" void kmain(uint32_t mbi, uint32_t bootloader_magic)
{
    /* Shall be called early on to allow interrupts */
    KIVirtualMemoryManager *kvmem = GetVirtualMemoryManager();
    KIInterruptManager *kim = GetInterruptManager();
    kim->install_vector();
	kim->add_interrupt_handler(14, page_fault_isr);
    KLegacyUart ttyS0(0);
    kernel_console = &ttyS0;

	Myclass c;

	kstd::kout.SetTextColour(BLACK);
	kstd::kout.SetBackColour(WHITE);
	kstd::kout.clear();

    kvmem->dump_virtual_mapping();

	//while(dbg);

	KIMultibootManager *mbmgr = kmmf.getMultibootMgr(bootloader_magic, (addr_t)mbi);
	if (!mbmgr)
	{
		kprintk("Was not able to get a multi-boot manager with magic 0x%x\n", bootloader_magic);
	}
	else
	{
		kusable_memory_region_t *regions = mbmgr->getMemoryRegions();
        kusable_memory_region_t *region = &regions[0];
		while (region->size != 0)
		{
			kprintk("Found a region from 0x%lx to 0x%lx -- size ",
                    region->base_address, region->base_address + region->size);
			if (region->size > GB) {
				kprintk("%d GiB\n", region->size/GB);
			}
			else if (region->size > MB) {
				kprintk("%d MiB\n", region->size/MB);
			}
			else if (region->size > KB) {
				kprintk("%d KiB\n", region->size/KB);
			}
			else {
				kprintk("%d Bytes\n", region->size);
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

    show_malloc_stats(ttyS0);

    uint64_t count = 0;
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
    kim->add_irq_handler(KEYBOARD_IRQ_NUMBER, GET_ISR_NAME(keyboard));
	kim->enableall();

	kstd::kout << _L("done, generates interrupt 0") << kstd::endl;
	kim->generate_interrupt(0);
	kstd::kout << _L("done, generates interrupt 2") << kstd::endl;
	kim->generate_interrupt(2);

	kstd::kout << _L("That's all folks!!") << kstd::endl;

    /* Go to sleep mode... */
    kprintk("&_start      0x%lx\n", &_start);
    kprintk("&_heap_start 0x%lx\n", &_heap_start);
    uint32_t stack_int;
    kprintk("&stack_int   0x%lx\n", &stack_int);
    kprintk("kim          %p\n", kim);
    while(1)
    {
        kprintk("Entering sleep...\n");
        __asm__("hlt");
        kprintk("Woken\n");
    }

	/* Test the crash handler
    int *ptr = (int *)0xFFFFFFFFA0000000;
	*ptr = 7;
	int test = *ptr;
	kstd::kout << _L("That's all folks!!") << test << kstd::endl;*/
}
