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
//   quit
//   cmd
//   clear
//   reboot
//
// BY: Francisco - gokernel@hotmail.com
//
//-------------------------------------------------------------------
//
#include "garden.h"
#include "multiboot.h"
#include "intr.h"   // interrupt API

#define STRING_SIZE   255
#define SHELL_PROMPT  "\nshell > "

char    string [STRING_SIZE];
int     quit;
int     key;
int     is_reboot = 0;

// prototypes:
//
void reboot (void);
void func   (int i);

struct CMD {
    char  *name;
    void  (*func) ();
    char  *help;
} function [] = {
    { "quit",   func,         "Halt the OS" },
    { "cmd",    func,         "Display the command line from Boot Loader" },
    { "clear",  video_clear,  "Clear the screen" },
    { "reboot", reboot,       "Reboot the OS" },
    { NULL, NULL, NULL }
};
#define FUNC_QUIT    0
#define FUNC_CMD     1

void func (int i) {
    if (i == FUNC_QUIT)
        quit = 1;

    if (i == FUNC_CMD) {
        if (var_Multi_Boot_Info) {
            printk ("\nMulti Boot cmdline(%s)\n", (char *) var_Multi_Boot_Info->cmdline);

            printk ("vbe_control_info : %d\n", var_Multi_Boot_Info->vbe_control_info);
            printk ("vbe_mode_info    : %d\n", var_Multi_Boot_Info->vbe_mode_info);
            printk ("vbe_mode         : %d\n", var_Multi_Boot_Info->vbe_mode);
            printk ("vbe_interface_seg: %d\n", var_Multi_Boot_Info->vbe_interface_seg);
            printk ("vbe_interface_off: %d\n", var_Multi_Boot_Info->vbe_interface_off);
            printk ("vbe_interface_len: %d\n", var_Multi_Boot_Info->vbe_interface_len);
        }
    }
}

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

    // display the function[] list
    //
    if (key == KEY_TAB) {
        struct CMD *cmd = function;

        video_puts ("\nCommands: ");
        while (cmd->name) {
            printk ("%s ", cmd->name);
            cmd++;
        }
        video_puts (SHELL_PROMPT);
        video_puts (string);
    }

    if (key == KEY_ENTER) {
        struct CMD *cmd = function;
        int count = 0;

        while (cmd->name) {

            if (!strcmp(string, cmd->name)) {

                cmd->func (count); // execute the command

                count = -1; // found

                break;
            }
            cmd++;
            count++;

        }//: while (cmd->name)

        if (count != -1) {
            video_puts ("\nCommand not found (");
            video_puts (string);
            video_puts (")\n");
        }

        memset (string, 0, STRING_SIZE);

        video_puts (SHELL_PROMPT);

    }//: if (key == KEY_ENTER)

}//: void keyboard_handle (int i)

void kmain (unsigned long arg) {

    kernel_init (arg);

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

