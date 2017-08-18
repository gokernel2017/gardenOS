//-------------------------------------------------------------------
//
// GARDEN OS:
//
//   LibC Implementation.
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
//   libc.c
//
// BY: Francisco - gokernel@hotmail.com
//
//-------------------------------------------------------------------
//
#include "garden.h"

static unsigned char *nextfree = (unsigned char*) 0x200000;

void *malloc (unsigned int amount) {
    void *retval = nextfree;

    nextfree += amount;

    return retval;
}


int strlen (const char *str) {
    register char *s = (char*)str;
    while (*++s) ;
    return s - str;
}

//-------------------------------------------------------------------
// Copyright (C) 2002 Alexander Blessing
// Contact email: Alexander.Blessing@epost.de
//
// This function is part of flick OS:
//   https://sourceforge.net/projects/flick/
//
// Function strcat():
//   adds src to dest
//
//-------------------------------------------------------------------
char *strcat (char *dest, const char *src) {
    char *s = dest;
	
    for (; *s != '\0'; s++); // go through dest until we reach a NUL

    while (*src != '\0')
		    *(s++) = *(src++);
    *s = '\0';
	
    return dest;
}

/* strcmp() 
 * compares two strings 
 */
int strcmp (const char *s1, const char *s2) {
    for (; *s1 == *s2; s1++, s2++)
        if (*s1 == '\0')
            return 0;	/* equal */
    return *s1 - *s2;
}

//-------------------------------------------------------------------
// Copyright (C) 2002 Alexander Blessing
// Contact email: Alexander.Blessing@epost.de
//
// This function is part of flick OS:
//   https://sourceforge.net/projects/flick/
//
// Function itoa():
//   convert an integer into a string
//
//-------------------------------------------------------------------
void itoa (unsigned long n, unsigned char *s, char base) {
    unsigned long i, j;
    unsigned char tmp[20], digit;

    i = 0;
    do {
        j = n;
        n /= base;	// divide num by base
        n *= base;	// multiply num by b to isolate the digit
        digit = j - n;
		
        // change the numerical digit to printable ASCII value
        if (digit <= 9) {
            tmp[i] = digit + '0';
        } else {
            tmp[i] = digit - 0x0A + 'A';
        }
        n /= base;	// divide num by b again to get to the next digit
        i++;
    } while (n != 0);
	
    tmp[i] = '\0'; // add the terminator
	
    // reverse string
    j = strlen(tmp) - 1;
    for (i = 0; i < strlen(tmp); i++) {
        s[i] = tmp[j];
        j--;
    }
    s[i] = '\0';
}

/* memset()
 * fills the first n bytes of s with the constant
 * byte c
 */
void *memset (void *s, int c, unsigned int n) {
    char *to = (char*) s;
    while (n-- > 0)
        *(to++) = c;
    return s;
}

/**
 * K&R implementation
 */
void int_to_ascii (int n, char str[]) {
    int i, sign;
    if ((sign = n) < 0) n = -n;
    i = 0;
    do {
        str[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);

    if (sign < 0) str[i++] = '-';
    str[i] = '\0';

    /* TODO: implement "reverse" */
}


void printk  (const char *format, ...) {
    unsigned int *argp = (void *)&format;
    char buf[20];

    while (1) {

        switch (*format) {
        case '\0':
            return;
        case '\n':
            video_puts ("\n");
            break;
        case '%':
            format++; argp++;
            switch (*format) {
            case '%': video_putc      ('%'); argp--;            break;
            case 'd': itoa (*argp, buf, 10); video_puts (buf);  break;
//            case 'x': /* falling through */
//            case 'p': vga_put_unsigned_num (*argp, 16);         break;
            case 'c': video_putc (*argp);                       break;
            case 's': video_puts ((char *)*argp);               break;
            //default:  panic ("printk");                         break;
            }
            break;
        default:
            video_putc (*format);
            break;
        }//: switch (*fmt)

        format++;

    }//: while (1)
}
