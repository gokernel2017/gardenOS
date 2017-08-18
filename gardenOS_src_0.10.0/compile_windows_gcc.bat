@echo off

gcc src/boot.S     -o obj/boot.o    -s -c -O2 -U_WIN32 -Wall -nostdinc -fno-exceptions -nostartfiles -ffreestanding -fno-builtin
gcc src/kernel.c   -o obj/kernel.o   -s -c -O2 -U_WIN32 -Wall -nostdinc -fno-exceptions -nostartfiles -ffreestanding -fno-builtin
gcc src/intr.c     -o obj/intr.o     -s -c -O2 -U_WIN32 -Wall -nostdinc -fno-exceptions -nostartfiles -ffreestanding -fno-builtin
gcc src/video.c    -o obj/video.o    -s -c -O2 -U_WIN32 -Wall -nostdinc -fno-exceptions -nostartfiles -ffreestanding -fno-builtin
gcc src/libc.c     -o obj/libc.o     -s -c -O2 -U_WIN32 -Wall -nostdinc -fno-exceptions -nostartfiles -ffreestanding -fno-builtin
gcc src/keyboard.c -o obj/keyboard.o -s -c -O2 -U_WIN32 -Wall -nostdinc -fno-exceptions -nostartfiles -ffreestanding -fno-builtin
gcc src/shell.c    -o obj/shell.o    -s -c -O2 -U_WIN32 -Wall -nostdinc -fno-exceptions -nostartfiles -ffreestanding -fno-builtin
gcc src/kmain.c    -o obj/kmain.o    -s -c -O2 -U_WIN32 -Wall -nostdinc -fno-exceptions -nostartfiles -ffreestanding -fno-builtin

ld -T src/link.ld -nostdlib obj/*.o -o obj/kernel.bin
objcopy -O binary obj/kernel.bin gardenOS_kernel
