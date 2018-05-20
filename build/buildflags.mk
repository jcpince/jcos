DEBUGFLAGS =	-g
#DEBUGFLAGS =
ARCHFLAGS =	-m64

CFLAGS =	${DEBUGFLAGS} ${ARCHFLAGS} -Wall -Wextra -Werror -fno-builtin
CFLAGS +=	-fno-stack-protector -ffreestanding -mcmodel=small -mno-red-zone
CFLAGS +=	-mno-mmx -mno-sse -mno-sse2 -mno-sse3 -mno-3dnow -fPIC
CFLAGS +=	-Wno-write-strings -fexceptions #-fno-rtti -fno-exceptions

CXXFLAGS =	${CFLAGS}

LDFLAGS =	${DEBUGFLAGS} -nodefaultlibs -nostdlib -nostartfiles

INCLUDES =	-nostdinc --sysroot ${ROOT} -isystem=/include

