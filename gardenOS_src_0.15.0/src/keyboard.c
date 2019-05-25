//-------------------------------------------------------------------
//
// GARDEN OS:
//
//   Keyboard Implementation.
//
// FILE:
//   keyboard.c
//
// BY: Francisco - gokernel@hotmail.com
//
//-------------------------------------------------------------------
//
#include "garden.h"

static int caps_lock_state = 0;
static int key_shift_state = 0;
static int state = 0;

// the keymap array
// this here is important - it maps the scancodes to ASCII
// we have one array for non-shifted and one for shifted
// keys
//
unsigned char keymap [][2] = {
/* 000 */ {0, 0},		
/* 001 */ {KEY_ESC, KEY_ESC},
/* 002 */ {'1', '!'}, 	
/* 003 */ {'2', '@'}, 
/* 004 */ {'3', '#'},
/* 005 */ {'4', '$'},
/* 006 */ {'5', '%'},
/* 007 */ {'6', '^'},
/* 008 */ {'7', '&'}, 
/* 009 */ {'8', '*'}, 
/* 010 */ {'9', '('}, 
/* 011 */ {'0', ')'},
/* 012 */ {'-', '_'}, 
/* 013 */ {'=', '+'}, 
/* 014 */ {KEY_BACKSPACE, KEY_BACKSPACE}, 
/* 015 */ {KEY_TAB, KEY_TAB},
/* 016 */ {'q', 'Q'}, 
/* 017 */ {'w', 'W'}, 
/* 018 */ {'e', 'E'},
/* 019 */ {'r', 'R'},
/* 020 */ {'t', 'T'},
/* 021 */ {'y', 'Y'},
/* 022 */ {'u', 'U'},
/* 023 */ {'i', 'I'},
/* 024 */ {'o', 'O'},
/* 025 */ {'p', 'P'}, 
/* 026 */ {'[', '{'}, 
/* 027 */ {']', '}'},
/* 028 */ {KEY_ENTER, KEY_ENTER},
/* 029 */ {KEY_CTRL, 0},
/* 030 */ {'a', 'A'},
/* 031 */ {'s', 'S'},
/* 032 */ {'d', 'D'},
/* 033 */ {'f', 'F'}, 
/* 034 */ {'g', 'G'},
/* 035 */ {'h', 'H'}, 
/* 036 */ {'j', 'J'},
/* 037 */ {'k', 'K'},
/* 038 */ {'l', 'L'}, 
/* 039 */ {';', ':'},
/* 040 */ {'\'', '"'},
/* 041 */ {'`', '~'},
/* 042 */ {KEY_LSHIFT, 0},
/* 043 */ {'\\', '|'},
/* 044 */ {'z', 'Z'},
/* 045 */ {'x', 'X'},
/* 046 */ {'c', 'C'},
/* 047 */ {'v', 'V'},
/* 048 */ {'b', 'B'},
/* 049 */ {'n', 'N'},
/* 050 */ {'m', 'M'},
/* 051 */ {',', '<'},
/* 052 */ {'.', '>'},
/* 053 */ {'/', '?'},
/* 054 */ {KEY_RSHIFT, 0},
/* 055 */ {0, 0},
/* 056 */ {KEY_ALT, 0},
/* 057 */ {KEY_SPACE, KEY_SPACE},
/* 058 */ {0, 0},

/* 059 */ {KEY_F1, KEY_F1},
/* 060 */ {KEY_F2, KEY_F2},
/* 061 */ {KEY_F3, KEY_F3}, 
/* 062 */ {KEY_F4, KEY_F4},
/* 063 */ {KEY_F5, KEY_F5},
/* 064 */ {KEY_F6, KEY_F6},
/* 065 */ {KEY_F7, KEY_F7}, 
/* 066 */ {KEY_F8, KEY_F8},
/* 067 */ {KEY_F9, KEY_F9},
/* 068 */ {KEY_F10, KEY_F10},
/* 069 */ {0, 0}, 
/* 070 */ {0, 0},

};

void keyboard_wait (void) {
    while ((inb(0x64) & 0x1) != 1) {

        // [@ wait about 1us for slow I/O. 0x80 is a port of DMA page register.
        // see also; http://www.linux.or.jp/JF/JFdocs/IO-Port-Programming/
        // high-resolution.html#AEN160.; cpu, delay, io]
        //
        // asm ("outb %al, $0x80");
				// outb(0, 0x80);
        kernel_wait ();
	    }
}

int keyboard_getkey (void) {
    unsigned char code;
    int release;

    code = inb (0x60);

    if (code > 0) {

        //-----------------------------
        // ON KEY OUT
        //-----------------------------
        //
        release = (code & 0x80) != 0;

        if (caps_lock_state == 0) {
            // on: key press
            if (code == KEY_LSHIFT || code == KEY_RSHIFT) key_shift_state = 1;
            // on: key out
            if (code == 170 || code == 182) key_shift_state = 0;
        } else {

            // on: key press
            if (code == KEY_LSHIFT || code == KEY_RSHIFT) state = 1;
            // on: key out
            if (code == 170 || code == 182) state = 0;

            //-----------------------------------
            // KEYS: 0 ... 13
            //-----------------------------------
            // 000 {0, 0},
            // 001 {KEY_ESC, KEY_ESC},
            // 002 {'1', '!'}, 	
            // 003 {'2', '@'}, 
            // 004 {'3', '#'},
            // 005 {'4', '$'},
            // 006 {'5', '%'},
            // 007 {'6', '^'},
            // 008 {'7', '&'}, 
            // 009 {'8', '*'}, 
            // 010 {'9', '('}, 
            // 011 {'0', ')'},
            // 012 {'-', '_'},
            // 013 {'=', '+'},
            //-----------------------------------
            //
            if (!release && (code <= 13 || code == 51 || code == 52 || code == 53)) {

        return keymap [ code ][ state ];	// BIG / small letters

            }
        }

        // fixa shift: CAIXA ALTA ou caixa baixa
        // caps lock
        if (code == 58) {
            if (caps_lock_state == 0) {
                caps_lock_state = 1;
                key_shift_state = 1;
            } else {
                caps_lock_state = 0;
                key_shift_state = 0;
            }
        }

        if (!release) {
            if (key_shift_state)
                return keymap[code][1];	// big letters
            else
                return keymap[code][0];	// small letters
        }

    }//: if (code > 0)

    return 0;
}

