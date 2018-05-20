include ${ROOT}/build/builddefs.mk
include ${ROOT}/build/buildflags.mk

SRCDIR = ${ROOT}/kernel
OBJDIR = ${ROOT}/obj/kernel

SRCS =	kmain.cpp \
	test.cpp \
	myclass.cpp \
	stdlib.c \
	stdio.c \
	string.c \
	ctype.c \
	unistd.c \
	pthread.cpp \
	KOStream.cpp \
	Video.cpp \
	icxxabi.cpp \
	KMultibootManager2.cpp
#	KMemoryManager.cpp

OBJECTS = $(addprefix ${OBJDIR}/, $(patsubst %.cc,%.o, $(patsubst %.cpp,%.o, $(patsubst %.c,%.o, $(patsubst %.S,%.o, ${SRCS})))))

all: ${OBJDIR}/built-in.o

include ${ROOT}/build/buildrules.mk

${OBJDIR}/built-in.o: ${OBJECTS} | ${OBJDIR}
	${LD} -r -o ${OBJDIR}/built-in.o ${OBJECTS}


