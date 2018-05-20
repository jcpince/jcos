OBJECTS = $(addprefix ${OBJDIR}/, $(patsubst %.cc,%.o, $(patsubst %.cpp,%.o, $(patsubst %.c,%.o, $(patsubst %.S,%.o, ${SRCS})))))

dis: ${DISASS}

ndis: ${NDISASS}

${OBJDIR}:
	${MKDIR} -p ${OBJDIR}

${OBJDIR}/%.o:${SRCDIR}/%.c | $(OBJDIR)
	${GCC} ${CFLAGS} ${INCLUDES} -o $@ -c $<
	
${OBJDIR}/%.o:${SRCDIR}/%.cc | $(OBJDIR)
	${GXX} ${CXXFLAGS} ${INCLUDES} -o $@ -c $<
	
${OBJDIR}/%.o:${SRCDIR}/%.cpp | $(OBJDIR)
	${GXX} ${CXXFLAGS} ${INCLUDES} -o $@ -c $<

${OBJDIR}/%.o:${SRCDIR}/%.S | $(OBJDIR)
	${GCC} ${CFLAGS} ${INCLUDES} -o $@ -c $<

${DISASS}: ${BINARY}
	${OBJDUMP} -D -b binary -m i8086 $< > $@

${NDISASS}: ${BINARY}
	${NDISASM} -b 16 $< > $@

force: ;
