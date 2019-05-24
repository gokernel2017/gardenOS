//-------------------------------------------------------------------
// Multiboot header, recognized by grub, must be aligned (see Kernel.ld)
// symbols for headers are defined in link.ld script
//
// This templant is part of PocketOS:
//
// http://pierrel5.free.fr/PocketOS/eindex.htm
//
// Thanks, Pierre !
//
//-------------------------------------------------------------------
//

#define MULTIBOOT_HEADER_MAGIC  0x1BADB002      // magic number for the Multiboot header.
#define MULTIBOOT_HEADER_FLAGS  0x00010002      // get memory info, no page align and not an elf format
#define STACK_TOP               0x80000         // position of stack at kernel init (changed later).

.code32    
.text
#ifdef WIN32
.extern _kernel_main
#endif
#ifdef __linux__
.extern kernel_main
#endif


multiboot_header:
    .long   MULTIBOOT_HEADER_MAGIC              // magic
    .long   MULTIBOOT_HEADER_FLAGS              // flags
    .long   -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)     // checksum
    .long   multiboot_header                    // header_addr
    .long   _ker_load_base_
    .long   _ker_load_end_                      // load_end_addr
    .long   _ker_bss_end_                       // bss_end_addr, use 0 if init done by kernel
    .long   multiboot_entry                     // entry_addr


multiboot_entry:

    movl    $(STACK_TOP), %esp                  // initialize the stack pointer.

    // ebx = address of multiboot_info struct
    //
    pushl   %ebx                                // load multiboot information

    cli                                         // disable interrupts.
    cld                                         // for gcc

    //-------------------------------------------
    // user main function:
    //-------------------------------------------
    //
#ifdef WIN32
    call _kernel_main
#endif
#ifdef __linux__
    call kernel_main
#endif


label_forever:
    hlt
    jmp label_forever


