//-------------------------------------------------------------------
//
// GARDEN OS:
//
//   The VGA Video Text Implementation.
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
//   video.c
//
// BY: Francisco - gokernel@hotmail.com
//
//-------------------------------------------------------------------
//
#include "garden.h"

#define VIDEO_SIZE        4000  // 80 * 25 * 2

#define REG_SCREEN_CTRL   0x3D4
#define REG_SCREEN_DATA   0x3D5

#define WHITE_TXT         0x07 // light gray on black text

static char   *video = (char*)0xb8000; // video memory begins at address 0xb8000
static int    pos = 0;
static int    line = 0;

static int    _pos_ = 0;
static int    _line_ = 0;


// Read a byte from the specified port
unsigned char inb (unsigned short port) {
    unsigned char result;
    asm ("inb %1, %0" :"=a" (result) :"d" (port));
    return result;
}


// Write a byte into the specified port
void outb (unsigned short port, unsigned char value) {
    asm ("outb %0, %1" : :"a" (value), "d" (port)); //: metalkit
}

// Set cursor position
// The same implementation as in get_cursor_offset()
// Write to CTRL register 14 (0xE) and write Hi byte to DATA register
// Write to CTRL register 15 (0xF) and write Lo byte to DATA register
static void set_cursor_position (int offset) {
    outb (REG_SCREEN_CTRL, 14);
    outb (REG_SCREEN_DATA, (unsigned char)(offset >> 8));
    outb (REG_SCREEN_CTRL, 15);
    outb (REG_SCREEN_DATA, (unsigned char)(offset & 0xFF));
}

void video_scroll (void) {
    register int i = 160;

    while (i < VIDEO_SIZE) {
        video [i-160] = video[i];
        i++;
    }

    // clear the last line
    i = (24*80*2);
    while (i < VIDEO_SIZE) {
        video [i] = ' ';
        video [i+1] = WHITE_TXT;
        i += 2;
    }
}

void video_putc (char c) {
    if (pos < VIDEO_SIZE) {
        video [pos] = c;
        video [pos+1] = WHITE_TXT;
        pos += 2;
    } else {
        line = 24;
        video_scroll ();
        pos = (line*80*2);
        video_putc (c);
    }
}

void video_puts (const char *s) {
    while (*s) {
        if (*s == '\n') {
            line++;
            pos = (line*80*2);
        } else {
            video_putc (*s);
        }
        s++;
    }
    set_cursor_position (pos/2);
}

void video_clear (void) {
    register int i = 0;
    while (i < VIDEO_SIZE) {
        video [i++] = ' ';
        video [i++] = WHITE_TXT;
    }
    pos = 0;
    line = 0;
}

void video_backspace (void) {
    if (pos >= 2) {
        pos -= 2;
        video [pos] = ' ';
        video [pos+1] = WHITE_TXT;
        set_cursor_position (pos/2);
    }
}

void video_goto_line (int y) {
    line = y;
    pos = (line*80*2);
}

void video_csave(void) {
    _pos_ = pos;
    _line_ = line;
}

void video_crestore (void) {
    pos = _pos_;
    line = _line_;
}
