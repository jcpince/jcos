include ${ROOT}/build/builddefs.mk
include ${ROOT}/build/buildflags.mk

SRCDIR = ${ROOT}/arch/x86_64
OBJDIR = ${ROOT}/obj/arch/x86_64

SRCS =	KInterruptManager.cpp isr64.S

OBJECTS = $(addprefix ${OBJDIR}/, $(patsubst %.cc,%.o, $(patsubst %.cpp,%.o, $(patsubst %.c,%.o, $(patsubst %.S,%.o, ${SRCS})))))

all: ${OBJDIR}/built-in.o

INCLUDES += -I.

include ${ROOT}/build/buildrules.mk

${OBJDIR}/built-in.o: ${OBJECTS} | ${OBJDIR}
	${LD} -r -o ${OBJDIR}/built-in.o ${OBJECTS}


