//-------------------------------------------------------------------
//
// GARDEN OS:
//
//   The VGA Video Text Implementation.
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

enum {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14,
    VGA_COLOR_WHITE = 15,
};

static char   *video = (char*)0xb8000; // video memory begins at address 0xb8000
static char   text_color = 0x07;
static int    video_count = 0;
static int    pos = 0;
static int    line = 0;

static int    _pos_ = 0;
static int    _line_ = 0;

unsigned char seconds, minutes, hours;


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

void video_put (void) {
    video [0] = 'A';
    video [1] = WHITE_TXT;
}

void video_putc (char c) {
    if (pos < VIDEO_SIZE) {
        video [pos] = c;
//        video [pos+1] = WHITE_TXT;
        video [pos+1] = text_color;
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

//
// NEED TIME ZONE IMPLEMENTATION ...
//
void video_display_time (void) {
//	setenv("TZ", "America/Sao_Paulo", 1);
    int save_line = line;
    int save_pos = pos;

    asm ("mov $0, %al");    // 0 = seconds
    asm ("out %al, $0x70");
    asm ("in $0x71, %al");
    #ifdef WIN32
    asm ("mov %al, _seconds");  // copy ( %al ) in var ( seg )
    #endif
    #ifdef __linux__
    asm ("mov %al, seconds");  // copy ( %al ) in var ( seg )
    #endif

    asm ("mov $2, %al");    // 2 = minutes
    asm ("out %al, $0x70");
    asm ("in $0x71, %al");
    #ifdef WIN32
    asm ("mov %al, _minutes");  // copy ( %al ) in var ( seg )
    #endif
    #ifdef __linux__
    asm ("mov %al, minutes");  // copy ( %al ) in var ( seg )
    #endif

    asm ("mov $4, %al");    // 2 = hours
    asm ("out %al, $0x70");
    asm ("in $0x71, %al");
    #ifdef WIN32
    asm ("mov %al, _hours");  // copy ( %al ) in var ( seg )
    #endif
    #ifdef __linux__
    asm ("mov %al, hours");  // copy ( %al ) in var ( seg )
    #endif

    text_color = VGA_COLOR_LIGHT_GREEN;

    pos = 0;
    line = 0;
    printk ("Time: %d:%d:%d | video_count : %d\n",
        ((hours   & 0x0F) + ((hours   / 16) * 10)),
        ((minutes & 0x0F) + ((minutes / 16) * 10)),
        ((seconds & 0x0F) + ((seconds / 16) * 10)),
        video_count++
        );
    line = save_line;
    pos = save_pos;
    set_cursor_position (pos/2);

    text_color = WHITE_TXT;
}


