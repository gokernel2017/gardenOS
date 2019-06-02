//-------------------------------------------------------------------
//
// GARDEN OS:
//
//   Main Header.
//
// THANKS TO:
// ----------------------------------------------
//   01: The creator of the heavens and the earth in the name of Jesus Christ.
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
//   garden.h
//
// GARDEN OS START DATE:
//   04/07/2017 - 03:02
//
// BY: Francisco - gokernel@hotmail.com
//
//-------------------------------------------------------------------
//
#ifndef _GARDEN_H_
#define _GARDEN_H_

#define USE_SUMMER_LANGUAGE 1

//-----------------------------------------------
//------------------  INCLUDE  ------------------
//-----------------------------------------------
//
#include "multiboot.h"
#include "initrd.h"
#include "lang_lex.h"
#include "lang_asm.h"
#include "lang_summer.h"

//-----------------------------------------------
//---------------  DEFINE / ENUM  ---------------
//-----------------------------------------------
//
#define GARDEN_VERSION        0
#define GARDEN_VERSION_SUB    15
#define GARDEN_VERSION_PATCH  2

#define NULL                  ((void*)0)
#define MAXLEN                1024
#define CLOCKS_PER_SEC        1000

// keys:
#define KEY_BACKSPACE         8
#define KEY_TAB               9
#define KEY_ENTER             13
#define KEY_ESC               27
#define KEY_CTRL              29
#define KEY_SPACE             32
#define KEY_LSHIFT            42
#define KEY_RSHIFT            54
#define KEY_ALT               56

#define KEY_F1                128
#define KEY_F2                129
#define KEY_F3                130
#define KEY_F4                131
#define KEY_F5                132
#define KEY_F6                133
#define KEY_F7                134
#define KEY_F8                135
#define KEY_F9                136
#define KEY_F10               137

#define WHITE_TXT             0x07 // light gray on black text

#define ARG_MAX               10

typedef char                  * va_list;
typedef __SIZE_TYPE__         size_t;
typedef unsigned char         Uchar;
typedef unsigned short        Ushort;


/* Amount of space required in an argument list for an arg of type TYPE.
   TYPE may alternatively be an expression whose type is used.  */

#define __va_rounded_size(TYPE)  \
  (((sizeof (TYPE) + sizeof (int) - 1) / sizeof (int)) * sizeof (int))

#define va_start(AP, LASTARG) \
 (AP = ((char *) &(LASTARG) + __va_rounded_size(LASTARG)))

extern void va_end (va_list);
#define va_end(AP) /* Nothing */

#define va_arg(AP, TYPE) (AP += __va_rounded_size (TYPE), \
  *((TYPE *) (AP - __va_rounded_size (TYPE))))


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

//-----------------------------------------------
//------------------  STRUCTS  ------------------
//-----------------------------------------------
//

typedef struct {
    int     argc;
    int     type [ARG_MAX + 1];
    VALUE   argv [ARG_MAX + 1];
}CMD_ARG;


// kernel.c
//
extern void kernel_main     (struct multiboot_info * mbi);
extern int  kernel_init     (struct multiboot_info * mbi);
extern void kernel_finalize (void);
extern void kernel_wait     (void);


// libc.c
//
extern int    strlen        (const char *str);
extern char * strcat        (char *dest, const char *src);
extern char	* strcpy        (char *dest, const char *src);
extern int    strcmp        (const char *s1, const char *s2);
extern char * strdup        (const char* str);
extern char * strchr        (const char *s, int c);
extern void   itoa          (unsigned long n, unsigned char *s, char base);
extern int    atoi          (const char *str);
extern void * memset        (void *s, int c, unsigned int n);
extern void * memcpy        (void *dest, const void *src, size_t n);
extern void   printk        (const char *fmt, ...);
// Copyright (C) 2002 Alexander Blessing
//   Alexander Blessing - Flick OS:
//   https://sourceforge.net/projects/flick/
extern void   sprintk       (char *str, const char *format, ...);
extern void   vsprintk      (char *str, const char *format, va_list ap);


// keyboard.c
//
extern void keyboard_wait   (void);
extern int  keyboard_getkey (void);


// video.c
//
extern Uchar  inb                 (unsigned short port);
extern Ushort inw                 (unsigned short port);
extern void   outb                (unsigned short port, unsigned char value);
//
extern void   video_clear         (void);
extern void   video_set_color     (unsigned char color);
extern void   video_put           (void);
extern void   video_putc          (char c);
extern void   video_puts          (const char *s);
extern void   video_backspace     (void);
extern void   video_goto_line     (int y);
extern void   video_csave         (void); // cursor save position
extern void   video_crestore      (void); // cursor restore position
extern void   video_scroll        (void);
extern void   video_display_time  (int value);


// kmalloc.c | MyOS_Blundell-master
//
extern void * kmalloc             (unsigned sz);
extern void   kfree               (void *fr);
extern void   kheap_SPEC          (void);


// initrd.c
//
//extern void   initrd              (struct multiboot_info * mbi);
extern fs_node_t * initrd (struct multiboot_info * mbi);

#endif // ! _GARDEN_H

