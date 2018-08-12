.code64
.section .text

.global kernel_start64                      /* making entry point visible to linker */
kernel_start64:
#if (EARLYDBG64 == 1)
1:  jmp  1b
#endif
    movabsq $KERNEL_PHYS_TO_VIRT, %rbx
    addq	%rbx, %rsp
    addq	%rbx, %rbp

    movabsq  $start_ctors, %rbx             /* call the constructors */
    movabsq  $end_ctors, %rcx
    jmp  2f
1:
    call *(%rbx)
    pop %rcx                                /* caller saved register */
    add  $8, %rbx
2:
    push %rcx
    cmp  %rcx, %rbx
    jb   1b
    pop %rcx

    movabsq $start_eh_frame, %rdi
    movabsq  $__register_frame, %rbx
    call *%rbx                              /* call __register_frame */

    pop %rbx                                /* get back the multiboot address and MAGIC from the stack */
    mov %ebx, %edi                          /* put the multiboot as the second argument */
    shr $32, %rbx                           /* 64-bit now so, shr 32 to get the upper bits */
    mov %ebx, %esi                          /* put the MAGIC as the first argument */

    movabsq   $kmain, %rax
    jmp    *%rax                            /* call kernel proper */

    movabsq   $end_dtors, %rbx              /* call the destructors */
    movabsq   $start_dtors, %rcx
    jmp  4f
3:
    sub  $8, %rbx
    call *(%rbx)
    pop %rcx
4:
    push %rcx                               /* caller saved register */
    cmp  %rcx, %rbx
    jb   3b
    pop %rcx

    cli
hang:
    hlt                                     /* halt machine should kernel return */
    jmp   hang
