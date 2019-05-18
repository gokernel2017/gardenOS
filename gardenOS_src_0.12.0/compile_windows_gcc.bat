@echo off

gcc src/boot.S     -m32 -o obj/boot.o     -s -c -O2 -U_WIN32 -Wall -nostdinc -fno-exceptions -nostartfiles -ffreestanding -fno-builtin
gcc src/kernel.c   -m32 -o obj/kernel.o   -s -c -O2 -U_WIN32 -Wall -nostdinc -fno-exceptions -nostartfiles -ffreestanding -fno-builtin
gcc src/intr.c     -m32 -o obj/intr.o     -s -c -O2 -U_WIN32 -Wall -nostdinc -fno-exceptions -nostartfiles -ffreestanding -fno-builtin
gcc src/video.c    -m32 -o obj/video.o    -s -c -O2 -U_WIN32 -Wall -nostdinc -fno-exceptions -nostartfiles -ffreestanding -fno-builtin
gcc src/libc.c     -m32 -o obj/libc.o     -s -c -O2 -U_WIN32 -Wall -nostdinc -fno-exceptions -nostartfiles -ffreestanding -fno-builtin
gcc src/sprintf.c  -m32 -o obj/sprintf.o  -s -c -O2 -U_WIN32 -Wall -nostdinc -fno-exceptions -nostartfiles -ffreestanding -fno-builtin
gcc src/kheap.c    -m32 -o obj/kheap.o    -s -c -O2 -U_WIN32 -Wall -nostdinc -fno-exceptions -nostartfiles -ffreestanding -fno-builtin
gcc src/keyboard.c -m32 -o obj/keyboard.o -s -c -O2 -U_WIN32 -Wall -nostdinc -fno-exceptions -nostartfiles -ffreestanding -fno-builtin
gcc src/shell.c    -m32 -o obj/shell.o    -s -c -O2 -U_WIN32 -Wall -nostdinc -fno-exceptions -nostartfiles -ffreestanding -fno-builtin
gcc src/kmain.c    -m32 -o obj/kmain.o    -s -c -O2 -U_WIN32 -Wall -nostdinc -fno-exceptions -nostartfiles -ffreestanding -fno-builtin

ld -T src/link.ld -nostdlib obj/*.o -o obj/kernel.bin
objcopy -O binary obj/kernel.bin gardenOS_kernel


