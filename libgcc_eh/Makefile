include ${ROOT}/build/builddefs.mk
include ${ROOT}/build/buildflags.mk

SRCDIR = ${ROOT}/libgcc_eh
OBJDIR = ${ROOT}/obj/libgcc_eh

SRCS =	unwind-dw2.c \
	unwind-c.c \
	unwind-dw2-fde-glibc.c

#CFLAGS = ${DEBUGFLAGS} ${ARCHFLAGS} -W -Wall -Wwrite-strings \
#	-Wcast-qual -Wstrict-prototypes \
#	-Wmissing-prototypes -Wold-style-definition \
#	-DIN_LIBGCC2 -D__GCC_FLOAT_NOT_NEEDED -fno-stack-protector \
#	-DENABLE_DECIMAL_BID_FORMAT -DHAVE_CC_TLS  -DUSE_TLS \
#	-fvisibility=hidden -DHIDE_EXPORTS

CFLAGS = ${DEBUGFLAGS} ${ARCHFLAGS} -W -Wall -Wwrite-strings \
	-Wcast-qual -DIN_LIBGCC2 -D__GCC_FLOAT_NOT_NEEDED -fno-stack-protector \
	-DENABLE_DECIMAL_BID_FORMAT -DHAVE_CC_TLS  -DUSE_TLS \
	-fvisibility=hidden -DHIDE_EXPORTS -fPIC

INCLUDES += -I${ROOT}/libgcc_eh

all: ${OBJDIR}/built-in.o

include ${ROOT}/build/buildrules.mk

${OBJDIR}/built-in.o: ${OBJECTS} | ${OBJDIR}
	${LD} -r -o ${OBJDIR}/built-in.o ${OBJECTS}

