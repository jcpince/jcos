include ${ROOT}/build/builddefs.mk
include ${ROOT}/build/buildflags.mk

SRCDIR = ${ROOT}/drivers/serial
OBJDIR = ${ROOT}/obj/drivers/serial

SRCS =	KSerial.cpp

OBJECTS = $(addprefix ${OBJDIR}/, $(patsubst %.cc,%.o, $(patsubst %.cpp,%.o, $(patsubst %.c,%.o, $(patsubst %.S,%.o, ${SRCS})))))

all: ${OBJDIR}/built-in.o

include ${ROOT}/build/buildrules.mk

${OBJDIR}/built-in.o: ${OBJECTS} | ${OBJDIR}
	${LD} -r -o $@ $^
