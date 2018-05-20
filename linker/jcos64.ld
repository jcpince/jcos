ENTRY (loader32)
OUTPUT_FORMAT(elf64-x86-64)

/*
 * Size constants
 */
KB =		1024;
MB =		(KB*KB);
GB =		(KB*MB);

/*
 * Virtual and Physical addresses of the kernel
 */
/*KERNEL_VIRT_ADDR =	0xFFFFFFFF80100000;*/
KERNEL_VIRT_ADDR =	0x80100000;
KERNEL_PHYS_ADDR =	(KERNEL_VIRT_ADDR & 0xFFFFFF);
KERNEL_PHYS_TO_VIRT =	(KERNEL_VIRT_ADDR - KERNEL_PHYS_ADDR);

/*
 * Various kernel buffers sizes and alignments
 */
SECTION_ALIGN =		(4*KB);
END_ALIGN =		(8*KB);
PAGING_BUF_ALIGN =	(4*KB);
PAGING_BUF_SIZE =	(36*KB);
CTORS_DTORS_ALIGN =	(32);
EXC_HDLR_FRAME_ALIGN =	(32);


PHDRS {
 bootstrap PT_LOAD FLAGS(5); /* R_E */
 text PT_LOAD FLAGS(5); /* R_E */
 data PT_LOAD FLAGS(6); /* RW_ */
}

SECTIONS
{
	/*
	 *	Here starts the physical address space
	 */
	. = KERNEL_PHYS_ADDR;

	.bootstrap ALIGN(SECTION_ALIGN) :
	{
		*(.bootstrap)

		. = ALIGN(PAGING_BUF_ALIGN);
		phys_pml4_start = .;
		. = phys_pml4_start + PAGING_BUF_SIZE;
		phys_pt_end = .;
	} : bootstrap

	/*
	 *	Here starts the virtual address space
	 */
	. += KERNEL_PHYS_TO_VIRT;

	.text ALIGN(SECTION_ALIGN) : AT(ADDR(.text) - KERNEL_PHYS_TO_VIRT)
	{
	_code = .;
	*(.text)
	*(.text.*)

	. = ALIGN(CTORS_DTORS_ALIGN);
	start_ctors = .;
	*(.ctor*)
	end_ctors = .;

	. = ALIGN(CTORS_DTORS_ALIGN);
	start_dtors = .;
	*(.dtor*)
	end_dtors = .;

	. = ALIGN(EXC_HDLR_FRAME_ALIGN);
	start_eh_frame = .;
	*(.eh_frame)
	QUAD(0)
	end_eh_frame = .;

	*(.rodata*)
	} : text

	.data ALIGN(SECTION_ALIGN) : AT(ADDR(.data) - KERNEL_PHYS_TO_VIRT)
	{
	_data = .;
	*(.data*)
	} : data

	.bss ALIGN(SECTION_ALIGN) : AT(ADDR(.bss) - KERNEL_PHYS_TO_VIRT)
	{
	_bss = .;
	*(.bss*)

	/*
	* You usually need to include generated COMMON symbols
	* under kernel BSS section or use gcc's -fno-common
	*/

	*(COMMON)
	} : data

	. = ALIGN(END_ALIGN);
	_end = .;

	/DISCARD/ :
	{
	*(.comment)
	*(.shstrtab)
	*(.symtab)
	*(.strtab)
	}
}

/* 
 *	Virtual and physical symbols translated
 */
virt_pml4_start = phys_pml4_start + KERNEL_PHYS_TO_VIRT;
virt_pt_end = phys_pt_end + KERNEL_PHYS_TO_VIRT;
stack_phys = stack - KERNEL_PHYS_TO_VIRT;

