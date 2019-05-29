@echo off

rem set OBJ_FILES = obj/boot.o obj/video.o obj/libc.o obj/interrupts.o obj/keyboard.o obj/kernel.o 

gcc src/boot.S     -o obj/boot.o     -s -c -O2 -U_WIN32 -Wall -nostdinc -fno-exceptions -nostartfiles -ffreestanding -fno-builtin
gcc src/video.c    -o obj/video.o    -s -c -O2 -U_WIN32 -Wall -nostdinc -fno-exceptions -nostartfiles -ffreestanding -fno-builtin
gcc src/libc.c     -o obj/libc.o     -s -c -O2 -U_WIN32 -Wall -nostdinc -fno-exceptions -nostartfiles -ffreestanding -fno-builtin
gcc src/intr.c     -o obj/intr.o     -s -c -O2 -U_WIN32 -Wall -nostdinc -fno-exceptions -nostartfiles -ffreestanding -fno-builtin
gcc src/keyboard.c -o obj/keyboard.o -s -c -O2 -U_WIN32 -Wall -nostdinc -fno-exceptions -nostartfiles -ffreestanding -fno-builtin
gcc src/kernel.c   -o obj/kernel.o   -s -c -O2 -U_WIN32 -Wall -nostdinc -fno-exceptions -nostartfiles -ffreestanding -fno-builtin

gcc src/kmalloc.c   -o obj/kmalloc.o -s -c -O2 -U_WIN32 -Wall -nostdinc -fno-exceptions -nostartfiles -ffreestanding -fno-builtin

gcc src/lang_lex.c  -o obj/lang_lex.o  -s -c -O2 -U_WIN32 -Wall -nostdinc -fno-exceptions -nostartfiles -ffreestanding -fno-builtin
gcc src/lang_asm.c  -o obj/lang_asm.o  -s -c -O2 -U_WIN32 -Wall -nostdinc -fno-exceptions -nostartfiles -ffreestanding -fno-builtin
gcc src/lang_core.c -o obj/lang_core.o -s -c -O2 -U_WIN32 -Wall -nostdinc -fno-exceptions -nostartfiles -ffreestanding -fno-builtin

ld -T src/link.ld -nostdlib obj/*.o -o obj/kernel.bin
objcopy -O binary obj/kernel.bin gardenOS_kernel

