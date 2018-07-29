cmake_minimum_required(VERSION 2.8)
project(jcos64)
enable_language(C CXX ASM)

if("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")
   message(FATAL_ERROR "Builds in sources are not allowed.")
endif()

set(CMAKE_DISABLE_SOURCE_CHANGES ON)

# Define features here
set(MEMORY_ALLOCATOR "dlmalloc")

set(_CMAKE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(_SRC_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(PROJECT_INCLUDES "")
set(PROJECT_SRC "")

include(${_CMAKE_ROOT}/macros.cmake)
include(${_CMAKE_ROOT}/flags.cmake)
include(${_CMAKE_ROOT}/sources.cmake)

foreach(INC ${PROJECT_INCLUDES})
  include_directories(${INC})
endforeach()

add_executable(${PROJECT_NAME}.elf ${PROJECT_SRC})
add_custom_command(TARGET ${PROJECT_NAME}.elf
    POST_BUILD
    COMMAND mkdir -p iso/boot/grub
    COMMAND cp ${PROJECT_NAME}.elf iso/boot
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/res/grub.cfg iso/boot/grub
    COMMAND grub-mkrescue -o jcos64.iso iso
    DEPENDS ${PROJECT_NAME}.elf ${CMAKE_CURRENT_SOURCE_DIR}/res/grub.cfg
)

add_custom_target(qemu-cd-serial-graphic
    COMMAND qemu-system-x86_64 -cdrom ${PROJECT_NAME}.iso -m 512 -serial stdio
    DEPENDS ${PROJECT_NAME}.elf
)

add_custom_target(qemu-cd-serial
    COMMAND qemu-system-x86_64 -cdrom ${PROJECT_NAME}.iso -m 512 -serial mon:stdio -nographic
    DEPENDS ${PROJECT_NAME}.elf
)

add_custom_target(qemu-cd-test
    COMMAND qemu-system-x86_64 -cdrom ${PROJECT_NAME}.iso -m 512
    DEPENDS ${PROJECT_NAME}.elf
)

add_custom_target(bochs-cd-test
    COMMAND bochs -f ${CMAKE_CURRENT_SOURCE_DIR}/res/bochs.cfg
    DEPENDS ${PROJECT_NAME}.elf
)

add_custom_target(qemu-cd-dbg-graphic
    COMMAND xterm -T "GDB client" -e gdb ${PROJECT_NAME}.elf &
	COMMENT "
    Set the architecture to i386 before establishing the connection
    Enter \"target remote :1234\" in the gdb prompt to connect the target
    When the x86-64 code is reached, set back the architecture to i386:x86-64
    Then, enter \"continue\" to continue the execution flow until it reached the loop in start.S
    if EARLYDBG is set, then, break and enter \"set \$pc=\$pc+2\" to leave the loop
    Then, enter \"b kmain\" to break from the very first instruction
    Finally, enter \"continue\" to continue the execution flow
    Enter Ctrl+a c to enter the Qemu interqctive shell."
	COMMAND qemu-system-x86_64 -cdrom ${PROJECT_NAME}.iso -m 512 -s -serial mon:stdio
    DEPENDS ${PROJECT_NAME}.elf
)

add_custom_target(qemu-cd-dbg
    COMMAND xterm -T "GDB client" -e gdb ${PROJECT_NAME}.elf &
	COMMENT "
    Set the architecture to i386 before establishing the connection
    Enter \"target remote :1234\" in the gdb prompt to connect the target
    When the x86-64 code is reached, set back the architecture to i386:x86-64
    Then, enter \"continue\" to continue the execution flow until it reached the loop in start.S
    if EARLYDBG is set, then, break and enter \"set \$pc=\$pc+2\" to leave the loop
    Then, enter \"b kmain\" to break from the very first instruction
    Finally, enter \"continue\" to continue the execution flow
    Enter Ctrl+a c to enter the Qemu interqctive shell."
	COMMAND qemu-system-x86_64 -cdrom ${PROJECT_NAME}.iso -m 512 -s -serial mon:stdio -nographic
    DEPENDS ${PROJECT_NAME}.elf
)

# Cleanup rules
set(additional_cleanup_files
    ${PROJECT_NAME}.map
    ${PROJECT_NAME}.iso
    iso/boot/${PROJECT_NAME}.elf
)

get_directory_property(clean_files ADDITIONAL_MAKE_CLEAN_FILES)
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES
    "${clean_files};${additional_cleanup_files}"
)

add_custom_target(distclean
    COMMAND ${CMAKE_BUILD_TOOL} clean
    COMMAND ${CMAKE_COMMAND} -E remove -f CMakeCache.txt
    COMMAND ${CMAKE_COMMAND} -E remove -f cmake_install.cmake
    COMMAND ${CMAKE_COMMAND} -E remove -f Makefile
    COMMAND rm -fr CMakeFiles
    COMMAND rm -fr iso
)
