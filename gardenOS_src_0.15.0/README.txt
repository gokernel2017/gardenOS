
 GARDEN OPERATING SYSTEM:

 A x86 (32 bits) Operating System in C.

 <<<<<<<<<<  THIS COMPILE ONLY TO 32 BITS  >>>>>>>>>>

 TO COMPILE IN WINDOWS TYPE:
   compile_windows_gcc.bat


 TO COMPILE IN LINUX TYPE:
   ./compile_linux_gcc.sh


 TO TEST WITH QEMU:
   qemu -initrd initrd -kernel gardenOS_kernel
 OR
   qemu-system-i386 -initrd initrd -kernel gardenOS_kernel


 TO TEST WITH GRUB( 1.x ):
 ----------------------------
 title GARDEN OS
   kernel /gardenOS_kernel
   module initrd
 ----------------------------


 BY: Francisco - gokernel@hotmail.com
