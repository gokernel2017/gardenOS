//-------------------------------------------------------------------
//
// GARDEN OS:
//
//   The Kernel.
//
// THANKS TO:
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

void init_lfb (struct multiboot_info *mbi) {
//	struct vbe_mode *mode_info;
//	void *lfb_ptr;

	if ( (mbi->flags & (1<<11)) == 0 )
	{
	/*
	    lfb_width  = 0;
	    lfb_height = 0;
	    lfb_depth = 0;
	    lfb_type = 0;
	    lfb_ptr = 0;
*/
			printk("VBE NOT INITIALIZED\n");
	}
	else
	{
//	    mode_info = (struct vbe_mode*) mbi->vbe_mode_info;
//	    printk("VBE w: %d", mode_info->x_resolution);
//	    printk("VBE h: %d", mode_info->y_resolution);
//	    lfb_depth = mode_info->bits_per_pixel;
//	    lfb_type = 0;
//	    lfb_ptr = (void*)mode_info->phys_base;

//			lfb = (ULONG*)lfb_ptr;
			printk("----------- VBE INITIALIZED ---------------\n");
	}
	
//	set_lfb( lfb_width, lfb_height, lfb_depth, lfb_type, lfb_ptr );
}

void kernel_init (unsigned long addr) {
    if (!init) {
        var_Multi_Boot_Info = (struct multiboot_info*) addr;
        init = 1;
        video_clear ();
        printk ("GARDEN OS ( %d.%d.%d ) - Build Release: %d\n",
                GARDEN_VERSION, GARDEN_VERSION_SUB, GARDEN_VERSION_PATCH, GARDEN_BUILD_RELEASE);

				init_lfb(var_Multi_Boot_Info);
    }
}

void kernel_finalize (void) {
    if (init) {
        video_puts ("\nKernel Finalize\n");
    }
}

void kernel_wait (void) {
		__asm__ __volatile__ ("hlt");
}

//#define halt_cpu() asm volatile("hlt")


