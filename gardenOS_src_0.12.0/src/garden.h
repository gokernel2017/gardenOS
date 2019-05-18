//-------------------------------------------------------------------
//
// GARDEN OS:
//
//   Main Header.
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
//   garden.h
//
// START DATE:
//   04/07/2017 - 03:02
//
// BY: Francisco - gokernel@hotmail.com
//
//-------------------------------------------------------------------
//
#ifndef _GARDEN_H_
#define _GARDEN_H_

#if defined(__x86_64__)
    #error "---------------------------------------------------------------"
    #error "SORRY: ----------  This project suport only 32 BITS  ----------"
    #error "---------------------------------------------------------------"
#endif

#define GARDEN_VERSION        0
#define GARDEN_VERSION_SUB    12
#define GARDEN_VERSION_PATCH  0

#define GARDEN_BUILD_RELEASE  2

#define NULL                  ((void*)0)
#define MAXLEN								1024

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

//
//-----------------------------------------------
//
//  _ASM("hlt");  // halt

#define _ASM __asm__ __volatile__

//
//-----------------------------------------------
//


//-----------------------------------------------
// TYPES:
//-----------------------------------------------
//
typedef unsigned char         Uchar;
typedef unsigned int					uint32_t;


//-----------------------------------------------
// GLOBAL IN FILE: kernel.c
//-----------------------------------------------
//
extern struct multiboot_info  *var_Multi_Boot_Info;


// kernel.c
//
extern void   kernel_init     (unsigned long arg);
extern void   kernel_finalize (void);
extern void   kernel_wait     (void);

// keyboard.c
//
extern int    keyboard_getkey (void);
extern void   keyboard_wait   (void);

// shell.c
extern int    shell           (char *string);

// libc.c
//
extern int    strlen          (const char *str);
extern char * strcat          (char *dest, const char *src);
extern int    strcmp          (const char *s1, const char *s2);
extern void   itoa            (unsigned long n, unsigned char *s, char base);
extern void * memset          (void *s, int c, unsigned int n);
extern void   printk          (const char *fmt, ...);

// video.c
//
extern void   video_clear     (void);
extern void   video_puts      (const char *s);
extern void   video_putc      (char c);
extern void   video_backspace (void);
extern void   video_goto_line (int y);
extern void   video_csave     (void); // cursor save position
extern void   video_crestore  (void); // cursor restore position
extern void   video_scroll    (void);

extern Uchar  inb             (unsigned short port);
extern void   outb            (unsigned short port, unsigned char value);

extern void * malloc          (unsigned int amount);

//
// kheap.c
//
extern void		kheap_SPEC			(void);
extern void * kmalloc					(unsigned sz);
extern void 	kfree						(void *fr);

//
// sprintf.c
//
extern int		sprintf					(char *str, const char *format, ...);

#endif // ! _GARDEN_H_

