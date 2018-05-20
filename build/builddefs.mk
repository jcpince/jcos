#CROSSTC =	i686-pc-jcos-

EARLYDBG ?= 0

ROOT =			${PWD}
INCDIR =		${ROOT}/include
LINKERDIR =		${ROOT}/linker
RESDIR =		${ROOT}/res
OBJDIR =		${ROOT}/obj
OUTDIR =		${ROOT}/out
ISODIR =		${OUTDIR}/iso

KERNEL =		${KERNELNAME}
BINARY =		${OUTDIR}/${KERNEL}.bin
ELF =			${OUTDIR}/${KERNEL}.elf
MAP = 			${OUTDIR}/${KERNEL}.map
ASM = 			${OUTDIR}/${KERNEL}.asm
ISO =			${OUTDIR}/${KERNEL}.iso
DISASS =		${OUTDIR}/${KERNEL}.dis
NDISASS =		${OUTDIR}/${KERNEL}.ndis
LINKERSCRIPT =	${LINKERDIR}/${KERNELNAME}.ld
GRUBCONF =		${RESDIR}/grub.cfg

DD =			dd
RM =			rm -f
CP =			cp -v
MKDIR =			mkdir
MKISOFS =		mkisofs
GRUBMKRESCUE =	grub-mkrescue

NASM =		nasm
NDISASM =	ndisasm
AS =		${CROSSTC}as
AS =		${CROSSTC}ar
RANLIB =	${CROSSTC}ranlib
LD =		${CROSSTC}ld
GCC =		${CROSSTC}g++
GXX =		${CROSSTC}g++
OBJDUMP =	${CROSSTC}objdump
