# JCOS, a C++ OS

Largely inspired by all the invaluable material found on https://wiki.osdev.org.

## Features:
- Virtual Memory Manager
- Interrupts manager
- Trap handler
- C++ exceptions support
- UART driver
- Screen driver

## How to build it:
```
mkdir build
cd build
cmake ..
```

## How to run it:
Can be ran in Qemu or Boschs using the make helpers qemu-cd-serial, qemu-cd-serial-graphic...
