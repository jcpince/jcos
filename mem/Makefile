include ${ROOT}/build/builddefs.mk
include ${ROOT}/build/buildflags.mk

SRCDIR = ${ROOT}/mem
OBJDIR = ${ROOT}/obj/mem

SRCS =	KMemPageHdlr.cpp

all: ${OBJDIR}/built-in.o

CFLAGS += -DEARLYDBG=${EARLYDBG} -I${INCDIR}/mem

include ${ROOT}/build/buildrules.mk	

${OBJDIR}/built-in.o: ${OBJECTS} | ${OBJDIR}
	${LD} -r -o ${OBJDIR}/built-in.o $^

