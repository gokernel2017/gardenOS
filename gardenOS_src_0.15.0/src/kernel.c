//-------------------------------------------------------------------
//
// GARDEN OS:
//
//   The Kernel Core.
//
// FILE:
//   kernel.c
//
// BY: Francisco - gokernel@hotmail.com
//
//-------------------------------------------------------------------
//
#include "garden.h"
#include "intr.h"   // interrupt API

#define STRING_SIZE     255
#define SHELL_PROMPT    "\nshell > "
#define FPS_50_PER_SEC  20

char    string_command [STRING_SIZE];
char    buf     [STRING_SIZE];

static unsigned long
    PIT_timer_ticks
    ;

static int
    quit, // to main loop quit
    key,  // key from KeyBoard
    is_reboot = 0
    ;

// prototypes:
void reboot (void);

struct CMD {
    char  *name;
    void  (*func) (void);
    char  *help;
} command [] = {
    { "clear",  video_clear,  "Clear the screen." },
    { "quit",   NULL,         "Halt the OS." },
    { "reboot", reboot,       "Reboot the OS." },
    { NULL, NULL, NULL }
};

// Read a byte from the specified port
inline unsigned char inb (unsigned short port) {
    unsigned char result;
    asm volatile ("inb %1, %0" : "=a" (result) : "d" (port));
    return result;
}

// Write a byte into the specified port
inline void outb (unsigned short port, unsigned char value) {
    asm volatile ("outb %0, %1" : :"a" (value), "d" (port));
}

static void kernel_init (struct multiboot_info *mbi) {
    video_clear();
    printk ("\nGarden OS: %d.%d.%d\n",
            GARDEN_VERSION, GARDEN_VERSION_SUB, GARDEN_VERSION_PATCH);

    video_puts ("GLORY TO GOD:\n  The creator of the heavens and the earth in the name of Jesus Christ. !\n\n");

    printk ("\nMulti Boot cmdline(%s)\n", (char *) mbi->cmdline);

    video_puts ("\nTo display the commands list press: KEY TAB\n");
}

static void kernel_finalize (void) {
    video_puts ("\nKernel Finalize\n");
}

void kernel_wait (void) {
    __asm__ __volatile__ ("hlt");
}

void reboot (void) {
    while (inb(0x64) & 0x02); // reboot the computer
    outb (0x64, 0xfe);

    // Should it not work we halt
    // the computer now so the we
    // do no harm
    //
    //_ASM ("cli;hlt");
    asm ("cli;hlt");

}//: void reboot (void)

static int ExecuteCommand (char *string) {
    static int pos = 0;
    int k;

    k = keyboard_getkey ();

    if (k > 0) {
        char buf [10] = { 0 };

        if (k == KEY_ENTER) {
            string [pos] = 0;
            pos = 0;
            return KEY_ENTER;
        } else if (k == KEY_BACKSPACE) {
            if (pos > 0) {
                pos--;
                video_backspace ();
            }
        } else if (k == KEY_TAB || k == KEY_ALT) {

            return k;

        } else if (k >= KEY_SPACE && k <= 126) { //  126 = ( ~ )
            string [pos++] = k;
            buf [0] = k;
            buf [1] = 0;
            video_puts (buf);
        } else {
            //return key;
        }
    }

    return 0;
}

void keyboard_handle (int i) {

    key = ExecuteCommand (string_command);

    // display the function[] list
    //
    if (key == KEY_TAB) {
        struct CMD *cmd = command;
        video_puts ("\nCommands: ");
        while (cmd->name) {
            printk ("%s ", cmd->name);
            cmd++;
        }
        video_puts (SHELL_PROMPT);
        video_puts (string_command);
        return;
    }

    if (key == KEY_ENTER) {
//        struct CMD *cmd = function;
        int i = 0;

        while (string_command[i]) {
            buf[i] = string_command[i];
            i++;
            if (string_command[i] == ' ') break;
        }
        buf[i] = 0;

        //---------------------------------------
        //------------  START EXECUTE  ----------
        //---------------------------------------
        if (!strcmp(buf, "quit")) {
            quit = 1;
        }
        if (!strcmp(buf, "clear")) {
            video_clear();
        }
        if (!strcmp(buf, "reboot")) {
            quit = is_reboot = 1;
            return;
        }

        //---------------------------------------
        //------------  END EXECUTE  ------------
        //---------------------------------------


        memset (string_command, 0, STRING_SIZE);

        video_puts (SHELL_PROMPT);

    }//: if (key == KEY_ENTER)
}

void PIT_timer_handler (int i) {

    PIT_timer_ticks++;

}

void kernel_main_loop (void) {
    int fps, count;

    video_puts (SHELL_PROMPT);

    quit = fps = count = 0;

    while (!quit) {

        fps++;

        // HERE: execute each 1 second:
        //
        if (PIT_timer_ticks % CLOCKS_PER_SEC == 0) {

            printk ("FPS: %d | count: %d\n", fps, count++); fps = 0;
            video_display_time ();

        }

        //
        // ... ! wait the next interrupts ...
        //
//        kernel_wait ();
    }
}

void kernel_main (struct multiboot_info *mbi) {

    kernel_init (mbi);

    intr_Init       (); // interrupt init
    intr_SetMask    (IRQ_KEYBOARD, TRUE); // enable keyboard interrupt
    intr_SetHandler (IRQ_VECTOR(IRQ_KEYBOARD), keyboard_handle); // set keyboard function handler

    intr_timer_InitPIT  (PIT_HZ / CLOCKS_PER_SEC); // 1000
    intr_SetMask        (IRQ_TIMER, TRUE);  // enable timer interrupt
    intr_SetHandler     (IRQ_VECTOR(IRQ_TIMER), PIT_timer_handler); // set timer function handler

    //-------------------------------------------
    // HERE EXECUTE:
    //
    //   01: PIT (Programmable Interval Timer) :=  PIT_timer_handler ()
    //
    //   02: The KeyBoard Interrupt Handler   := keyboard_handle ()
    //
    //   03: AND FINALLY | The Main Loop
    //
    //-------------------------------------------
    //
    kernel_main_loop ();

    intr_Disable ();

    kernel_finalize ();

    if (is_reboot)
        reboot ();
}


