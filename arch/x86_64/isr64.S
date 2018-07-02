.code64
.section .text

/* TODO: Include a common header containing this */
#define	KSEG_DS	0x10

/* Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax */
#define pusha64 \
	push %rdi; \
	push %rsi; \
	push %rbp; \
	push %rsp; \
	push %rbx; \
	push %rdx; \
	push %rcx; \
	push %rax

/* Pops edi,esi,ebp,esp,ebx,edx,ecx,eax */
#define popa64 \
	pop %rax; \
	pop %rcx; \
	pop %rdx; \
	pop %rbx; \
	pop %rsp; \
	pop %rbp; \
	pop %rsi; \
	pop %rdi

#define install_kds64 \
	mov %ds, %rax; \
	push %rax; \
	mov $KSEG_DS, %eax; \
	mov %eax, %ds; \
	mov %eax, %es; \
	mov %eax, %fs; \
	mov %eax, %gs

/* reload the original data segment descriptor */
#define restaure_origds \
	pop %rax; \
	mov %eax, %ds; \
	mov %eax, %es; \
	mov %eax, %fs; \
	mov %eax, %gs

#define install_isr(isr_name)                   \
    .extern callback_##isr_name;                \
    .globl  isr_##isr_name;                     \
    isr_##isr_name:                             \
        cli;                                    \
        pusha64;                                \
        install_kds64;                          \
        movabsq   $callback_##isr_name, %rax;   \
        call    *%rax;                          \
        restaure_origds;                        \
        popa64;                                 \
        sti;                                    \
        iretq;   /* pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP */

install_isr(keyboard)
install_isr(int0)
install_isr(int1)
install_isr(int2)
install_isr(int3)
install_isr(serial)

.extern page_fault_callback
.globl page_fault_isr
page_fault_isr:
	cli
	pusha64
	install_kds64

	movq	%cr2, %rdi
	movq	0x50(%rsp), %rsi
	movq	0x48(%rsp), %rdx
	movabsq   $page_fault_callback, %rax
	call    *%rax

	restaure_origds
	popa64
	sti
	iretq           /* pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP */
