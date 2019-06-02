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
//
char *strcat (char *dest, const char *src) {
    char *s = dest;
	
    for (; *s != '\0'; s++); // go through dest until we reach a NUL

    while (*src != '\0')
        *(s++) = *(src++);
    *s = '\0';
	
    return dest;
}

/*
// strcpy() 
// copies src to dest
//
char *strcpy (char *dest, const char *src) {
		while (*src != '\0')
				*(dest++) = *(src++);
		return dest;
}
*/

// Copy the NULL-terminated string src into dest, and
// return dest.
char *strcpy(char *dest, const char *src) {
    do {
        *dest++ = *src++;
    }
    while (*src != 0);
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

char * strdup (const char* str) {
    size_t length = strlen(str);
    char* result = kmalloc(length + 1);
    memcpy(result, str, length + 1);
    return result;
}

char * strchr (const char *s, int c) {
    while (*s && *s != c)
        s++;
    return *s || !c? s: NULL;
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
//
/*
static int next;
static char qbuf[8];

char *itoa(n)
int n;
{
  register int r, k;
  int flag = 0;

  next = 0;
  if (n < 0) {
	qbuf[next++] = '-';
	n = -n;
  }
  if (n == 0) {
	qbuf[next++] = '0';
  } else {
	k = 10000;
	while (k > 0) {
		r = n/k;
		if (flag || r > 0) {
			qbuf[next++] = '0' + r;
			flag = 1;
		}
		n -= r * k;
		k = k/10;
	}
  }
  qbuf[next] = 0;
  return(qbuf);
}
*/
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

/*
// denix
void* memcpy(void* restrict dest, const void* restrict src, size_t size) {
    unsigned char* d = dest;
    const unsigned char* s = src;

    for (size_t i = 0; i < size; i++) {
        d[i] = s[i];
    }

    return dest;
}
*/

void *memcpy (void *dest, const void *src, size_t n) {
    char *to = (char*) dest, *from = (char*) src;
    while (n-- > 0)
        *(to++) = *(from++);
    return dest;
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


void printk (const char *format, ...) {
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

}//: void printk (const char *format, ...)


/*
 * lib/vsprintf.c
 * Copyright (C) 2002 Alexander Blessing
 *
 * this file only contains the quite important function vsprintf.c
 * This function is totally written by me - not as powerful as Linus'
 * but it's my own one :-)
 */

/*
#include <stdio.h>
#include <string.h>
#include <stdarg.h>	// for va_list and va_arg 
//#include <kernel/interrupt.h>
*/

void vsprintk (char *str, const char *format, va_list ap) {
	char *p = str;
	char *s; int d; char c; char buf[MAXLEN];
	int i = 0;
	
	*p = '\0';
	/* go through format until we reach a % */
	while (*format != '\0')
	{
		if (*format != '%') {	/* ordinary character */
			i = strlen(p);
			p[i] = *format;
			p[i + 1] = '\0';
		}
		
		else {
			switch (*(format + 1)) {
				case 'c':	/* char */
					c = (char) va_arg(ap, int);
					i = strlen(p);
					p[i] = c;
					p[i + 1]  = '\0';
					break;		
				case 's':	/* string */
					s = va_arg(ap, char *);
					strcat(p, s);
					break;
				case 'd':	/* integer */
				case 'i':
					d = va_arg(ap, int);
					itoa(d, buf, 10);	/* convert to string */
					strcat(p, buf);
					break;
				case 'x':	/* hexa-decimal */
					d = va_arg(ap, int);
					itoa(d, buf, 16);
					strcat(p, buf);
					break;
				default:
					/* print the % and the following character */
					i = strlen(p);
					p[i]  = *format;
					p[i + 1]  = *(format + 1);
					p[i + 2]  = '\0';
					break;
			}
			format++;
		}
		format++;
	}
}

void sprintk (char *str, const char *format, ...) {
    va_list ap;
    va_start (ap, format);
    vsprintk (str, format, ap);
    va_end (ap);
    //return strlen (str);
}


/*
 * Copyright (c) 2015-2018, Davide Galassi. All rights reserved.
 *
 * This file is part of the BeeOS software.
 *
 * BeeOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with BeeOS; if not, see <http://www.gnu/licenses/>.
 */


#define isspace(c) 	((int) ((c) == ' '  || (c) == '\n' || (c) == '\f' \
						 || (c) == '\r' || (c) == '\t' || (c) == 'v'))

/** Test for a decimal digit. */
#define isdigit(c) 	((int) ((c) >= '0' && (c) <= '9'))


int atoi (const char *str) {
	int i;
	int sign = +1;
	int num;

	for (i = 0; isspace(str[i]); i++);

	if (str[i] == '+')
	{
		i++;
	}
	else if (str[i] == '-')
	{
		sign = -1;
		i++;
	}

	for (num = 0; isdigit(str[i]); i++)
	{
		num = (num << 3)+(num << 1);	/* num *= 10 */
		num += (str[i] - '0');
	}

	if (sign == -1)
	{
		num = -num;
	}

	return num;
}
