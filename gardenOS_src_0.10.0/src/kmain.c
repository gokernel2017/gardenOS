//-------------------------------------------------------------------
//
// GARDEN OS
//
// Main User Function:
//   void kmain (void);
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
//   kmain.c
//
// MINI SHELL COMMANDS:
//   clear
//   scroll
//   quit
//   reboot
//
// BY: Francisco - gokernel@hotmail.com
//
//-------------------------------------------------------------------
//
#include "garden.h"
#include "intr.h"   // interrupt API

#define STRING_SIZE   255
#define SHELL_PROMPT  "\nshell > "

char    string [STRING_SIZE];
int     quit;
int     key;
int     is_reboot = 0;

void reboot (void) {
    while (inb(0x64) & 0x02); // reboot the computer
    outb (0x64, 0xfe);

    // Should it not work we halt
    // the computer now so the we
    // do no harm
    //
    asm ("cli;hlt");
}

void keyboard_handle (int i) {

    key = shell (string);

    if (key == KEY_TAB) {
        video_puts ("\nCommands: clear, quit, reboot, scroll");
        video_puts (SHELL_PROMPT);
        video_puts (string);
    }

    if (key == KEY_ENTER) {
        if (!strcmp(string, "clear")) {
            video_clear ();
        } else if (!strcmp(string, "quit")) {
            quit = 1;
        } else if (!strcmp(string, "reboot")) {
            quit = 1;
            is_reboot = 1;
        } else if (!strcmp(string, "scroll")) {
            video_scroll();
        } else {
            video_puts ("\nCommand not found (");
            video_puts (string);
            video_puts (")\n");
        }
        memset (string, 0, STRING_SIZE);

        video_puts (SHELL_PROMPT);

    }//: if (key == KEY_ENTER)

}//: void keyboard_handle (int i)

void kmain (void) {

    kernel_init ();

    intr_Init (); // interrupt init
    intr_SetMask (IRQ_KEYBOARD, TRUE); // enable keyboard interrupt
    intr_SetHandler (IRQ_VECTOR(IRQ_KEYBOARD), keyboard_handle); // set keyboard handler

    video_puts ("\nGLORY TO GOD:\nThe creator of the heavens and the earth in the name of Jesus Christ. !\n");

    video_puts ("\nTo display the commands list press: KEY TAB\n");

    video_puts (SHELL_PROMPT);

    quit = 0;

    //-------------------------------------------
    // MAIN LOOP:
    //
    // See the function:
    //
    //   void keyboard_handle (int i)
    //
    //-------------------------------------------
    //
    while (!quit) {

        kernel_wait ();

    }

    intr_Disable ();

    kernel_finalize ();

    if (is_reboot)
        reboot();
}

