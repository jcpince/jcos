include ${ROOT}/build/builddefs.mk
include ${ROOT}/build/buildflags.mk

SRCDIR = ${ROOT}/init
OBJDIR = ${ROOT}/obj/init

SRCS =	start.S bootstrap.S

all: ${OBJDIR}/built-in.o

#all: ${OBJDIR}/built-in.o ${OBJDIR}/bootstrap.o

#${OBJDIR}/bootstrap.o: ${SRCDIR}/bootstrap.S

# bootstrap.o shall not be in built-in.o because
# it is referenced directly in the linker script
# so, don't put it into SRCS list
#${OBJDIR}/bootstrap.o:${SRCDIR}/bootstrap.S

CFLAGS += -DEARLYDBG=${EARLYDBG} -I${INCDIR}/init

include ${ROOT}/build/buildrules.mk	

${OBJDIR}/built-in.o: ${OBJECTS} | ${OBJDIR}
	${LD} -r -o ${OBJDIR}/built-in.o $^

