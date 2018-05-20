include ${ROOT}/build/builddefs.mk
include ${ROOT}/build/buildflags.mk

SRCDIR = ${ROOT}/libcxxrt
OBJDIR = ${ROOT}/obj/libcxxrt

SRCS =	dynamic_cast.cpp \
	exception.cpp \
	guard.cpp \
	stdexcept.cc \
	typeinfo.cc \
	memory.cc \
	auxhelper.cc \
	libelftc_dem_gnu3.c

INCLUDES += -I${ROOT}/libcxxrt

all: ${OBJDIR}/built-in.o

include ${ROOT}/build/buildrules.mk

${OBJDIR}/built-in.o: ${OBJECTS} | ${OBJDIR}
	${LD} -r -o ${OBJDIR}/built-in.o ${OBJECTS}

		
