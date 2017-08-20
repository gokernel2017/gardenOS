//-------------------------------------------------------------------
//
// GARDEN OS:
//
//   The Shell Implementation.
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
//   shell.c
//
// BY: Francisco - gokernel@hotmail.com
//
//-------------------------------------------------------------------
//
#include "garden.h"

int shell (char *string) {
    static int pos = 0;
    int key;

    key = keyboard_getkey ();

    if (key > 0) {
        char buf [10] = { 0 };

        if (key == KEY_ENTER) {
            string [pos] = 0;
            pos = 0;
            return KEY_ENTER;
        } else if (key == KEY_BACKSPACE) {
            if (pos > 0) {
                pos--;
                video_backspace ();
            }
        } else if (key == KEY_TAB || key == KEY_ALT) {

            return key;

        } else if (key >= KEY_SPACE && key <= 126) { //  126 = ( ~ )
            string [pos++] = key;
            buf [0] = key;
            buf [1] = 0;
            video_puts (buf);
        } else {
            //return key;
        }
    }

    return 0;
}
