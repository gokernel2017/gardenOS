//-------------------------------------------------------------------
//
// GARDEN OS:
//
//   The Kernel.
//
// TANKS TO:
// ----------------------------------------------
//   01: God the creator of the heavens and the earth in the name of Jesus Christ.
//
//   02: OsDev Site: www.osdev.org
//
//   03: Alexander Blessing - Flick OS:
//       https://sourceforge.net/projects/flick/
//       Contact email: Alexander.Blessing@epost.de
//
//   03: Micah Dowty - Creator of MetalKit:
//       https://github.com/scanlime/metalkit/
//       http://svn.navi.cx/misc/trunk/metalkit/
// ----------------------------------------------
//
// FILE:
//   kernel.c
//
// BY: Francisco - gokernel@hotmail.com
//
//-------------------------------------------------------------------
//
#include "garden.h"
#include "multiboot.h"

//-----------------------------------------------
// GLOBAL:
//-----------------------------------------------
//
struct multiboot_info *var_Multi_Boot_Info = NULL;

static int init = 0;

void kernel_init (unsigned long addr) {
    if (!init) {
        var_Multi_Boot_Info = (struct multiboot_info*) addr;
        init = 1;
        video_clear ();
        printk ("GARDEN OS ( %d.%d.%d ) - Build Release: %d\n",
                GARDEN_VERSION, GARDEN_VERSION_SUB, GARDEN_VERSION_PATCH, GARDEN_BUILD_RELEASE);
    }
}

void kernel_finalize (void) {
    if (init) {
        video_puts ("\nKernel Finalize\n");
    }
}

void kernel_wait (void) {
    //_ASM ("hlt");
    asm ("hlt");
}

