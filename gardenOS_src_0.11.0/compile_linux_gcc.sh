gcc -c src/boot.S     -o obj/boot.o     -O2 -Wall -nostdlib -nostdinc
gcc -c src/kernel.c   -o obj/kernel.o   -O2 -Wall -nostdlib -nostdinc
gcc -c src/intr.c     -o obj/intr.o     -O2 -Wall -nostdlib -nostdinc
gcc -c src/video.c    -o obj/video.o    -O2 -Wall -nostdlib -nostdinc
gcc -c src/libc.c     -o obj/libc.o     -O2 -Wall -nostdlib -nostdinc
gcc -c src/keyboard.c -o obj/keyboard.o -O2 -Wall -nostdlib -nostdinc
gcc -c src/shell.c    -o obj/shell.o    -O2 -Wall -nostdlib -nostdinc
gcc -c src/kmain.c    -o obj/kmain.o    -O2 -Wall -nostdlib -nostdinc

ld -T src/link.ld -nostdlib obj/*.o -o obj/kernel.bin
objcopy -O binary obj/kernel.bin gardenOS_kernel

