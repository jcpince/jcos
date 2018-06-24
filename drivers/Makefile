include ${ROOT}/build/builddefs.mk
include ${ROOT}/build/buildflags.mk

SUBDIRS = serial dummy

OBJDIR = ${ROOT}/obj/drivers

SUBBUILTINS = $(addsuffix /built-in.o, $(addprefix ${OBJDIR}/, ${SUBDIRS}))

all: ${OBJDIR}/built-in.o

%/built-in.o: force
	${MAKE} -C $(subst ${OBJDIR}/,,$(subst /built-in.o,,$@)) ROOT=${ROOT}

include ${ROOT}/build/buildrules.mk

${OBJDIR}/built-in.o: ${SUBBUILTINS} | ${OBJDIR}
	${LD} -r -o $@ $<
