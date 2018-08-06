set(CMAKE_C_COMPILER gcc-6)

set(CMAKE_EXE_LINKER_FLAGS "-g -nodefaultlibs -nostdlib")
append(CMAKE_EXE_LINKER_FLAGS "-nostartfiles -Map=${PROJECT_NAME}.map")

set(CMAKE_C_FLAGS "-ggdb3 -m64 -Wall -Wextra -Werror -fno-builtin")
append(CMAKE_C_FLAGS "-fno-stack-protector -ffreestanding -mcmodel=small -mno-red-zone")
append(CMAKE_C_FLAGS "-mno-mmx -mno-sse -mno-sse2 -mno-sse3 -mno-3dnow -fPIC")
append(CMAKE_C_FLAGS "-Wno-write-strings -fexceptions -nostdinc --sysroot")
append(CMAKE_C_FLAGS "${CMAKE_CURRENT_SOURCE_DIR} -isystem=/include") #-fno-rtti -fno-exceptions
append(CMAKE_C_FLAGS "-Wwrite-strings -Wcast-qual -fvisibility=hidden -fPIC")

set(EARLYDBG 0 CACHE STRING 0)
add_definitions(-DEARLYDBG=${EARLYDBG})

add_definitions(-DIN_LIBGCC2 -D__GCC_FLOAT_NOT_NEEDED)
add_definitions(-DENABLE_DECIMAL_BID_FORMAT -DHAVE_CC_TLS -DUSE_TLS -DHIDE_EXPORTS)

set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11")

set(CMAKE_CXX_LINK_EXECUTABLE "ld ${CMAKE_EXE_LINKER_FLAGS} -o ${PROJECT_NAME}.elf -T ${CMAKE_CURRENT_SOURCE_DIR}/linker/${PROJECT_NAME}.ld <OBJECTS>")
