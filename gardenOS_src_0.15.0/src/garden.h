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

//-----------------------------------------------
//------------------  INCLUDE  ------------------
//-----------------------------------------------
//
#include "multiboot.h"

//-----------------------------------------------
//---------------  DEFINE / ENUM  ---------------
//-----------------------------------------------
//
#define GARDEN_VERSION        0
#define GARDEN_VERSION_SUB    15
#define GARDEN_VERSION_PATCH  0

#define NULL                  ((void*)0)
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


// kernel.c
//
extern void kernel_main     (struct multiboot_info *mbi);
extern void kernel_wait     (void);

 // Read a byte from the specified port
extern inline unsigned char inb   (unsigned short port);
 // Write a byte into the specified port
extern inline void          outb  (unsigned short port, unsigned char value);

// video.c
//
extern void video_put       (void);
extern void video_putc      (char c);
extern void video_puts      (const char *s);
extern void video_clear     (void);
extern void video_backspace (void);
extern void video_display_time (void);
// libc.c
//
extern void printk          (const char *format, ...);
extern int  strcmp          (const char *s1, const char *s2);
extern void *memset         (void *s, int c, unsigned int n);


// keyboard.c
//
extern void keyboard_wait   (void);
extern int  keyboard_getkey (void);

#endif // ! _GARDEN_H

