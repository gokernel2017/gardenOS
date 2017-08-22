//-------------------------------------------------------------------
//
// GARDEN OS:
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

char    string  [STRING_SIZE];
char    buf     [STRING_SIZE];
//
int     quit;
int     key;
int     is_reboot = 0;
int     func_index;

unsigned char seconds, minutes, hours;

// prototypes:
//
void reboot     (void);
void func       (void);
void help       (void);
void func_time  (void); // NEED TIME ZONE IMPLEMENTATION ...

struct CMD {
    char  *name;
    void  (*func) (void);
    char  *help;
} function [] = {
    { "quit",   func,         "Halt the OS." },
    { "cmd",    func,         "Display the command line from Boot Loader." },
    { "help",   help,         "Display Information about commands ..." },
//    { "time",   func_time,    "Display the Time Clock" },
    { "clear",  video_clear,  "Clear the screen." },
    { "reboot", reboot,       "Reboot the OS." },
    { NULL, NULL, NULL }
};
enum {
    FUNC_QUIT = 0,
    FUNC_CMD,
};
void func (void) {
    if (func_index == FUNC_QUIT) {
        quit = 1;
        return;
    }

    if (func_index == FUNC_CMD) {
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

}//: void func (void)

void help (void) {
    int i = 0, count = 5;
    struct CMD *cmd = function;

    if (!strcmp(string, "help")) { // no argument: display all commands
        while (cmd->name) {
            printk ("\n%s: %s", cmd->name, cmd->help);
            cmd++;
        }
        return;
    }

    buf[0] = 0;
    if (strlen(string) > 5) {
        // ! set argument 1 in buf[]
        //
        while (string[count]) {
            buf[i++] = string[count++];
            if (string[count] == ' ') break;
        }
        buf[i] = 0;
        if (buf[0]) {

            while (cmd->name) { // find with argument
                if (!strcmp(buf, cmd->name) && cmd->help) {
                    printk ("\n%s: %s\n", buf, cmd->help);
              return;
                }
                cmd++;
            }

            if (buf[1] != 0) {
                video_puts ("\nHelp Usage:\nhelp\nhelp c\nhelp command\n");
          return;
            }

            cmd = function;
            while (cmd->name) { // find from the first letter of argument
                if (buf[0] == cmd->name[0] && cmd->help) {
                    printk ("\n%s: %s", cmd->name, cmd->help);
                }
                cmd++;
            }

        }//: if (buf[0])

    }//: if (strlen(string) > 5)

}//: void help (void)

//
// NEED TIME ZONE IMPLEMENTATION ...
//
void func_time (void) {
/*
    asm ("mov $0, %al");    // 0 = seconds
    asm ("out %al, $0x70");
    asm ("in $0x71, %al");
    asm ("mov %al, _seconds");  // copy ( %al ) in var ( seg )

    asm ("mov $2, %al");    // 2 = minutes
    asm ("out %al, $0x70");
    asm ("in $0x71, %al");
    asm ("mov %al, _minutes");  // copy ( %al ) in var ( seg )

    asm ("mov $4, %al");    // 2 = hours
    asm ("out %al, $0x70");
    asm ("in $0x71, %al");
    asm ("mov %al, _hours");  // copy ( %al ) in var ( seg )

    printk ("\ntime: %d:%d:%d\n",
        ((hours   & 0x0F) + ((hours   / 16) * 10)),
        ((minutes & 0x0F) + ((minutes / 16) * 10)),
        ((seconds & 0x0F) + ((seconds / 16) * 10))
        );
*/
}

void reboot (void) {
    while (inb(0x64) & 0x02); // reboot the computer
    outb (0x64, 0xfe);

    // Should it not work we halt
    // the computer now so the we
    // do no harm
    //
    asm ("cli;hlt");
}//: void reboot (void)

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
        int i = 0;

        while (string[i]) {
            buf[i] = string[i];
            i++;
            if (string[i] == ' ') break;
        }
        buf[i] = 0;

        func_index = 0;
        
        while (cmd->name) {

            if (!strcmp(buf, cmd->name)) {
                cmd->func (); // execute the command
                func_index = -1;    // found
                break;
            }
            cmd++;
            func_index++;

        }//: while (cmd->name)

        if (func_index != -1) {
            video_puts ("\nCommand not found (");
            video_puts (buf);
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

}//: void kmain (unsigned long arg)

