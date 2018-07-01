#include <MyClass.hpp>
#include <KOStream.hpp>
#include <kernel/KMemoryManager.hpp>
#include <init/KMultibootManagerFactory.hpp>
#include <KIInterruptManager.hpp>
#include <drivers/serial/KSerial.hpp>

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

int dbg = 1;

extern "C" void myisr0();
extern "C" void myisr1();
extern "C" void myisr2();
extern "C" void myisr3();
extern "C" void kbdisr();
extern "C" void page_fault_isr();
/*
extern "C" void __cxa_pure_virtual()
{
    abort();
}

void* operator new(size_t size)
{
	void * mem = malloc(size);
	if (!mem)
	{
		abort();
	}

	return mem;
}

void operator delete(void * ptr)
{
	free(ptr);
}*/

extern "C" void kbd_callback(void)
{
	uint8_t key, scancode;
	bool	keypressed;
	__asm__ __volatile__ (
		"in $0x60, %%al		\n" /* read information from the keyboard */
		"mov %%al, %0		\n"
		"mov $0x20, %%al	\n"
		"out %%al, $0x20	\n" /* acknowledge the interrupt to the PIC */
		: "=r" (key)
	);
	keypressed = (key & 0x80) == 0;
	scancode = key & 0x7f;
	printk("Keyboard interrupt: got 0x%x %s: code 0x%x '%c'\n", key&0xff, (keypressed ? "pressed" : "released"), scancode, scancode);
}

extern "C" void int0_callback(void)
{
	printk("Interrupt 0 caught\n");
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

extern "C" void int1_callback(void)
{
	printk("Interrupt 1 caught\n");
}

extern "C" void int2_callback(void)
{
	printk("Interrupt 2 caught\n");
}

extern "C" void int3_callback(void)
{
	printk("Interrupt 3 caught\n");
}

extern "C" void kmain(uint32_t mbi, uint32_t bootloader_magic)
{
	Myclass c;
	uint64_t count = 0;
    KSerial ttyS0(0);

	kstd::kout.SetTextColour(BLACK);
	kstd::kout.SetBackColour(WHITE);
	kstd::kout.clear();

	//while(dbg);

	KIMultibootManager *mbmgr = kmmf.getMultibootMgr(bootloader_magic, (addr_t)mbi);
	if (!mbmgr)
	{
		kstd::kout.printf("Was not able to get a multi-boot manager with magic 0x%x\n", bootloader_magic);
	}
	else
	{
		kusable_memory_region_t *regions = mbmgr->getMemoryRegions();
		while (regions->size != 0)
		{
			kstd::kout.printf("Found a region from 0xlx to %lx -- size ",
                    regions->base_address, regions->base_address + regions->size);
			if (regions->size > GB) {
				kstd::kout.printf("%d GiB\n", regions->size/GB);
			}
			else if (regions->size > MB) {
				kstd::kout.printf("%d MiB\n", regions->size/GB);
			}
			else if (regions->size > KB) {
				kstd::kout.printf("%d KiB\n", regions->size/GB);
			}
			else {
				kstd::kout.printf("%d Bytes\n", regions->size/GB);
			}
			regions++;
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

	kstd::kout << _L("setup the idt and waits for 2s") << kstd::endl;
	kim->install_vector();
	kim->add_handler(0, myisr0);
	kim->add_handler(1, myisr1);
	kim->add_handler(2, myisr2);
	kim->add_handler(3, myisr3);
	kim->add_handler(14, page_fault_isr);

	kstd::kout << _L("done, enable interrupts...") << kstd::endl;
	kim->enableall();
	kstd::kout << _L("done, generates interrupt 0") << kstd::endl;
	kim->generate_interrupt(0);
	kstd::kout << _L("done, generates interrupt 1") << kstd::endl;
	kim->generate_interrupt(1);
	kstd::kout << _L("done, generates interrupt 2") << kstd::endl;
	kim->generate_interrupt(2);
	kstd::kout << _L("done, generates interrupt 3") << kstd::endl;
	kim->generate_interrupt(3);

	kstd::kout << _L("done, install a keyboard interrupt handler") << kstd::endl;
	uint32_t flags = kim->savei();
	kim->disablei();
	__asm__ volatile (
		"mov $0xff, %al \n"
		"out %al, $0xA1 \n"
		"mov $0xfd, %al \n"
		"out %al, $0x21 \n"
	);
	kim->add_handler(9, kbdisr);
	kim->restaurei(flags);
	kstd::kout << _L("done, ready to receive keys") << kstd::endl;

	kstd::kout << _L("That's all folks!!") << kstd::endl;


	kstd::kout.printf("sizeof(unsigned int): %d\n", sizeof(unsigned int));
	kstd::kout.printf("sizeof(unsigned long): %d\n", sizeof(unsigned long));
	kstd::kout.printf("sizeof(unsigned long int): %d\n", sizeof(unsigned long int));

    int cd = 100;
    while (cd-- > 0)
    {
        char ch = ttyS0.read();
        kstd::kout.printf("received %c\n", ch);
        ttyS0.write(ch+1);
    }

	sleep(5);
	int *ptr = (int *)0xFFFFFFFFA0000000;
	*ptr = 7;
	int test = *ptr;
	kstd::kout << _L("That's all folks!!") << test << kstd::endl;
	//kstd::kout << _L("That's all folks!!") << kstd::endl;
	while(1);
}
