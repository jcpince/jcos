KERNELNAME =	jcos64
PWDP	=	$(shell pwd -P)
VERSION = 	$(subst $(shell dirname ${PWDP})/${KERNELNAME}-,,${CURDIR})

include build/builddefs.mk
include build/buildflags.mk

SUBDIRS = init arch mem kernel drivers libcxxrt libgcc_eh

SUBBUILTINS = $(addsuffix /built-in.o, $(addprefix ${OBJDIR}/, ${SUBDIRS}))

all: ${ISO} ${ASM}

${ASM}: ${ELF}
	${OBJDUMP} -D -l $< > $@

${OUTDIR}:
	${MKDIR} -p ${OUTDIR}

%/built-in.o: force
	${MAKE} -C $(subst ${OBJDIR}/,,$(subst /built-in.o,,$@)) ROOT=${ROOT} EARLYDBG=${EARLYDBG}

force: ;

${ELF}: ${SUBBUILTINS} ${LINKERSCRIPT} | ${OUTDIR}
	@echo "CURDIR is ${CURDIR}"
	@echo "PWD is ${PWD}"
	@echo "PWDL is ${PWDL}"
	@echo "PWDP is ${PWDP}"
	@echo "KERNELNAME is ${KERNELNAME}"
	@echo "VERSION is ${VERSION}"
	@echo "ISODIR is ${ISODIR}"
	@echo "ELF is ${ELF}"
	@echo "BINARY is ${BINARY}"
	@echo "LINKERSCRIPT is ${LINKERSCRIPT}"
	@echo "OUTDIR is ${OUTDIR}"
	#${GXX} ${LDFLAGS} ${ARCHFLAGS} -Xlinker -Map=${MAP} -T ${LINKERSCRIPT} -o $@ ${SUBBUILTINS}
	${LD} ${LDFLAGS} -T ${LINKERSCRIPT} -o $@ ${SUBBUILTINS} -Map=${MAP}

${BINARY}: ${OBJECTS} ${LINKERSCRIPT} | ${OUTDIR}
	${LD} ${LDFLAGS} -T ${LINKERSCRIPT} -o $@ ${OBJECTS}

${ISO}: ${ELF} ${GRUBCONF}
	${RM} -r ${ISODIR}
	${MKDIR} -p ${ISODIR}/boot/grub
	${CP} ${GRUBCONF} ${ISODIR}/boot/grub
	${CP} ${ELF} ${ISODIR}/boot/${KERNELNAME}.elf
	${GRUBMKRESCUE} -o $@ ${ISODIR}

qemu-cd-test: ${ISO}
	qemu-system-x86_64 -cdrom ${ISO} -m 512

qemu-cd-serial: ${ISO}
	qemu-system-x86_64 -cdrom ${ISO} -m 512 -serial stdio

bochs-cd-test: ${ISO}
	bochs -f res/bochs.cfg

qemu-cd-dbg: ${ISO} ${ELF}
	xterm -T "GDB client" -e gdb ${ELF} &
	@echo "\033[1m"
	@echo "\tSet the architecture to i386 before establishing the connection"
	@echo "\tEnter \"target remote :1234\" in the gdb prompt to connect the target"
	@echo "\tWhen the x86-64 code is reached, set back the architecture to i386:x86-64"
	@echo "\tThen, enter \"continue\" to continue the execution flow until it reached the loop in start.S"
	@echo "\tif EARLYDBG is set, then, break and enter \"set \$$pc=\$$pc+2\" to leave the loop"
	@echo "\tThen, enter \"b kmain\" to break from the very first instruction"
	@echo "\tFinally, enter \"continue\" to continue the execution flow"
	@echo ""
	@echo "\033[0m"
	qemu-system-x86_64 -cdrom ${ISO} -m 512 -s -monitor stdio

clean:
	${RM} -fr ${OBJDIR} $(OUTDIR)

cleanall:
	${RM} -fr ${OBJDIR} $(OUTDIR)
	find . -name "*~" -exec ${RM} {} \;
