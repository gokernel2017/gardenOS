//00000000000000000000000000000000000000000000000000000

//-------------------------------------------------------------------
//
// GARDEN OS:
//
//   The Kernel Core.
//
// MAIN FUNCTION:
//
//   void kernel_main ( struct multiboot_info * mbi );
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

#define SHELL_PROMPT    "\nshell > "
#define FPS_50_PER_SEC  20

//
// Enable/Disable in file: garden.h
//
#ifdef USE_SUMMER_LANGUAGE
    //
    // The JIT | Summer Language:
    //
    static ASM * asm_main = NULL;

#endif // USE_SUMMER_LANGUAGE

static LEXER lexer;

static char
    string_command [MAXLEN + 1]
    ;

static unsigned long
    PIT_timer_ticks
    ;

static int
    quit, // to main loop quit
    is_reboot = 0
    ;

// prototypes:
int GetLineCommand (char *string);
void cmd_echo (void);
int store_arg (CMD_ARG *arg);

struct CMD {
    char  *name;
    void  (*func) ( );
    char  *help;
} command [] = {
    { "quit",     NULL,         "Halt the OS." },
    { "reboot",   NULL,         "Reboot the OS." },
    { "clear",    video_clear,  "Clear the screen." },
    { "echo",     cmd_echo,     "Dysplay a text message" },
    { NULL, NULL, NULL }
};


void funcao (int a, int b, int c) { //
    printk("a: %d\n", a);
    printk("b: %d\n", b);
    printk("c: %d\n", c);
    printk("Result: %d\n", a+b+c);
}

void cmd_echo (void) {
    CMD_ARG arg;
    store_arg (&arg);
    printk("\nCOMMAND ECHO %d\n", arg.argc);
    funcao (arg.argv[0].l, arg.argv[1].l, arg.argv[2].l);
}

int store_arg (CMD_ARG *arg) {
    int i = 0;
    lex_set (&lexer, string_command, "string");
    lex (&lexer);
    while (lex(&lexer) && i < ARG_MAX) {
        if (lexer.tok==TOK_NUMBER) {
            arg->argv[i].l = atoi (lexer.token);
            arg->type[i] = TOK_NUMBER;
            i++;
        }
    }
    return (arg->argc = i);
}

int kernel_init (struct multiboot_info * mbi) {

    video_clear();

    kheap_SPEC();

    #ifdef USE_SUMMER_LANGUAGE
    //
    // Summer Language Init:
    //
    if ((asm_main = core_Init (5000)) == NULL)
  return 0;
    #endif

    printk ("\nWITH ROOTFS | Garden OS: %d.%d.%d\n",
            GARDEN_VERSION, GARDEN_VERSION_SUB, GARDEN_VERSION_PATCH);

    video_set_color (VGA_COLOR_LIGHT_BROWN);

    video_puts ("GLORY TO GOD:\n  The creator of the heavens and the earth in the name of Jesus Christ. !\n\n");

    printk ("\nMulti Boot cmdline(%s)\n", (char *) mbi->cmdline);

    video_set_color (WHITE_TXT);

    video_puts ("\nTo display the commands list press: KEY TAB\n");

    return 1;
}

void kernel_finalize (void) {
    video_puts ("\nKernel Finalize\n");
}

void kernel_wait (void) {
    asm volatile ("hlt");
}

void reboot (void) {
    while (inb(0x64) & 0x02); // reboot the computer
    outb (0x64, 0xfe);

    // Should it not work we halt
    // the computer now so the we
    // do no harm
    //
    asm volatile ("cli;hlt");

}// reboot ()


int GetLineCommand (char *string) {
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

    switch (GetLineCommand(string_command)) {
    case KEY_TAB: { // display the function[] list
        struct CMD *cmd = command;
        video_puts ("\nCommands: ");
        video_set_color(VGA_COLOR_LIGHT_MAGENTA);
        while (cmd->name) {
            printk ("%s ", cmd->name);
            cmd++;
        }
        video_set_color(WHITE_TXT);
        video_puts (SHELL_PROMPT);
        video_puts (string_command);
        return;
        } 

    case KEY_ENTER: {
        struct CMD *cmd = command;
        char buf [MAXLEN + 1];
        int i = 0, exist;

        #ifdef USE_SUMMER_LANGUAGE
        //---------------------------------------
        //
        // Summer Language ... Execute JIT:
        //
        //---------------------------------------
        //
        if (string_command[0]=='$') {
            if (core_Parse (&lexer, asm_main, string_command, "string")==0) {
                printk ("\n");
                asm_Run (asm_main); //<<<<<<<<<<  RUN JIT  >>>>>>>>>>
            }
            else printk ("ERRO:\n%s\n", ErroGet());
            memset (string_command, 0, MAXLEN);
            video_puts (SHELL_PROMPT);
      return;
        }
        #endif // USE_SUMMER_LANGUAGE

        // set ( buf ) with the first "string"
        //
        while (string_command[i]) {
            buf[i] = string_command[i];
            i++;
            if (string_command[i] == ' ') break;
        }
        buf[i] = 0;

        // ! find if command exist
        i = 0;
        exist = -1;
        while (cmd->name) {
            if (!strcmp(cmd->name, buf)) {
                exist = i; // <<<<<<<<<<  OK FUNCTION EXIST  >>>>>>>>>>
                break;
            }
            i++;
            cmd++;
        }
        // command not found:
        if (exist == -1) {
            video_set_color(VGA_COLOR_LIGHT_CYAN);
            printk ("\nCommand Not Found: '%s'", buf);
            memset (string_command, 0, MAXLEN);
            video_set_color(WHITE_TXT);
            video_puts (SHELL_PROMPT);
      return;
        }

        //---------------------------------------
        //------------  START EXECUTE  ----------
        //---------------------------------------

        if (!strcmp(buf, "quit")) {
            quit = 1;
      return;
        }
        if (!strcmp(buf, "reboot")) {
            quit = is_reboot = 1;
      return;
        }

        //
        // ... Execute The Function ...
        //
        if (exist != -1 && command[exist].func != NULL) {
            command[exist].func();
        }

        //---------------------------------------
        //------------  END EXECUTE  ------------
        //---------------------------------------

        memset (string_command, 0, MAXLEN);
        video_puts (SHELL_PROMPT);

        }// case KEY_ENTER:
    }// switch (GetLineCommand(string_command))
}// keyboard_handle()


void PIT_timer_handler (int i) {

    PIT_timer_ticks++;

}


void kernel_main_loop (void) {
    int fps = 0;

    video_puts (SHELL_PROMPT);

    quit = 0;

    while (!quit) {
	
        // HERE: execute 50 time in 1 second:
        //
        // 1000 / 20 := 50
        //
        if (PIT_timer_ticks % FPS_50_PER_SEC == 0) {

            fps++; 

        }

        // HERE: execute each 1 second:
        //
        if (PIT_timer_ticks % CLOCKS_PER_SEC == 0) {

            video_display_time (fps);
            fps = 0;

        }

        //
        // ... ! wait the next interrupts ...
        //
        kernel_wait ();
    }

}// kernel_main_loop ()


int name, data, size, xx;
char buf [10] = { 0, 0, 0, 0, 0 };

void my_initrd (struct multiboot_info * mbi) {
    if (mbi->mods_count > 0) {
        int location = *((unsigned int*)mbi->mods_addr);
        char id    = *(unsigned int *)location;
        int nfiles = *(unsigned int *)(location + 1);
        int pos = 5, i;

        printk ("InitRD Started ...\nID: %d\nNFILES: %d\n", id, nfiles);
        for (i = 0; i < nfiles; i++) {
                if (i == 0) {
                    name = *(unsigned int *)(location + pos);
                    data = *(unsigned int *)(location + pos+5);
                    size = *(unsigned int *)(location + pos+9);
                    pos += 13;
                } else {
                    name = *(unsigned int *)(location + pos);
                    data = *(unsigned int *)(location + pos+4);
                    size = *(unsigned int *)(location + pos+8);
                    pos += 12;
                }
//            printk ("\nname: %d | data: %d | size: %d\n", name, data, size);
            // data:
            xx = 6 + (nfiles * 12) + name;
            int count = data-name;
            printk ("File(");
            while (count--) {
                buf[0] = *(unsigned int *)(location + xx);
                printk ("%c", buf[0]);
                xx++;
            }
            size--;
            printk(") size: %d\n", size);
            // data:
            xx = 6 + (nfiles * 12) + data;
            while (size--) {
                buf[0] = *(unsigned int *)(location + xx);
                printk ("%c", buf[0]);
                xx++;
            }
            printk("\n");
        }
    } else {
        printk ("INITRD NOT FOUND\n");
    }
}

void kernel_main (struct multiboot_info * mbi) {

    if (!kernel_init(mbi))
  return;

    Intr_Init       (); // interrupt init
    Intr_SetMask    (IRQ_KEYBOARD, TRUE); // enable keyboard interrupt
    Intr_SetHandler (IRQ_VECTOR(IRQ_KEYBOARD), keyboard_handle); // set keyboard function handler

    Intr_Timer_InitPIT  (PIT_HZ / CLOCKS_PER_SEC); // 1000
    Intr_SetMask        (IRQ_TIMER, TRUE);  // enable timer interrupt
    Intr_SetHandler     (IRQ_VECTOR(IRQ_TIMER), PIT_timer_handler); // set timer function handler

    my_initrd(mbi);

/*
    // ... testing ...
    //
    if ((fs_root = initrd (mbi)) != NULL) {
        printk ("-------- INIT FS INITIALIZED -----------\n");
        // list the contents of /
        int i = 0;
        struct dirent *node = 0;

        while ((node = readdir_fs(fs_root, i)) != 0) {

            printk ("Found file(%d): '%s'", node->ino, node->name);
            fs_node_t *fsnode = finddir_fs(fs_root, node->name);

            if ((fsnode->flags&0x7) == FS_DIRECTORY) {
                video_puts ("\n\t(directory)\n");
            } else {
                printk ("\n\t contents: \"");
                char buf[256];
                unsigned int sz = read_fs(fsnode, 0, 256, buf);
                int j;
                for (j = 0; j < sz; j++)
                    video_putc (buf[j]);
            
                video_puts("\"\n");
            }

            i++;
        }
    }
*/

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

    Intr_Disable ();

    kernel_finalize ();

    if (is_reboot)
        reboot ();
}


