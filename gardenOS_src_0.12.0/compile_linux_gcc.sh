gcc -c src/boot.S     -m32 -o obj/boot.o     -O2 -Wall -nostdlib -nostdinc
gcc -c src/kernel.c   -m32 -o obj/kernel.o   -O2 -Wall -nostdlib -nostdinc
gcc -c src/intr.c     -m32 -o obj/intr.o     -O2 -Wall -nostdlib -nostdinc
gcc -c src/video.c    -m32 -o obj/video.o    -O2 -Wall -nostdlib -nostdinc
gcc -c src/libc.c     -m32 -o obj/libc.o     -O2 -Wall -nostdlib -nostdinc
gcc -c src/sprintf.c  -m32 -o obj/sprintf.o  -O2 -Wall -nostdlib -nostdinc
gcc -c src/kheap.c    -m32 -o obj/kheap.o    -O2 -Wall -nostdlib -nostdinc
gcc -c src/keyboard.c -m32 -o obj/keyboard.o -O2 -Wall -nostdlib -nostdinc
gcc -c src/shell.c    -m32 -o obj/shell.o    -O2 -Wall -nostdlib -nostdinc
gcc -c src/kmain.c    -m32 -o obj/kmain.o    -O2 -Wall -nostdlib -nostdinc

ld -T src/link.ld -nostdlib obj/*.o -o obj/kernel.bin
objcopy -O binary obj/kernel.bin gardenOS_kernel

