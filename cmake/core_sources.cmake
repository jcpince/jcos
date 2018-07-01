# Assembly code
append_list(PROJECT_SRC
    ${_SRC_ROOT}/arch/x86_64/isr64.S
    ${_SRC_ROOT}/init/bootstrap.S
    ${_SRC_ROOT}/init/start.S
)

# Kernel proper
append_list(PROJECT_SRC
    ${_SRC_ROOT}/kernel/kmain.cpp
    ${_SRC_ROOT}/kernel/icxxabi.cpp
    ${_SRC_ROOT}/kernel/KOStream.cpp
    ${_SRC_ROOT}/kernel/Video.cpp
    ${_SRC_ROOT}/kernel/KMultibootManager2.cpp
    ${_SRC_ROOT}/kernel/MyClass.cpp
    ${_SRC_ROOT}/arch/x86_64/KInterruptManager.cpp
)

# Standard libc
append_list(PROJECT_SRC
    ${_SRC_ROOT}/kernel/stdio.c
    ${_SRC_ROOT}/kernel/stdlib.c
    ${_SRC_ROOT}/kernel/string.c
    ${_SRC_ROOT}/kernel/ctype.c
    ${_SRC_ROOT}/kernel/unistd.c
    ${_SRC_ROOT}/kernel/pthread.cpp
)

# CXX lib
append_list(PROJECT_SRC
    ${_SRC_ROOT}/libcxxrt/dynamic_cast.cc
	${_SRC_ROOT}/libcxxrt/exception.cc
	${_SRC_ROOT}/libcxxrt/guard.cc
	${_SRC_ROOT}/libcxxrt/stdexcept.cc
	${_SRC_ROOT}/libcxxrt/typeinfo.cc
	${_SRC_ROOT}/libcxxrt/memory.cc
	${_SRC_ROOT}/libcxxrt/auxhelper.cc
	${_SRC_ROOT}/libcxxrt/libelftc_dem_gnu3.c
)

# CXX Exception lib
append_list(PROJECT_SRC
    ${_SRC_ROOT}/libgcc_eh/unwind-dw2.c
    ${_SRC_ROOT}/libgcc_eh/unwind-c.c
    ${_SRC_ROOT}/libgcc_eh/unwind-dw2-fde-glibc.c
)
