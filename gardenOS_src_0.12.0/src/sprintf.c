/*
 * lib/vsprintf.c
 * Copyright (C) 2002 Alexander Blessing
 *
 * this file only contains the quite important function vsprintf.c
 * This function is totally written by me - not as powerful as Linus'
 * but it's my own one :-)
 */
#include "garden.h"

//#include <string.h>
//#include <stdarg.h>	/* for va_list and va_arg */
//#include <kernel/interrupt.h>


/*
 * include/stdarg.h
 * Copyright (C) 2002 Alexander Blessing
 * 
 * defines useful functions for retrieving a unknown amount
 * of parameters
 */

//#ifndef _INCLUDE_STDARG_H_
//#define _INCLUDE_STDARG_H_

/* these functions are from the official GNU stdarg.h file */

typedef char *kva_list;

/* Amount of space required in an argument list for an arg of type TYPE.
   TYPE may alternatively be an expression whose type is used.  */

#define __kva_rounded_size(TYPE)  \
  (((sizeof (TYPE) + sizeof (int) - 1) / sizeof (int)) * sizeof (int))

#define kva_start(AP, LASTARG) \
 (AP = ((char *) &(LASTARG) + __kva_rounded_size(LASTARG)))

extern void kva_end (kva_list);
#define kva_end(AP) /* Nothing */

#define kva_arg(AP, TYPE) (AP += __kva_rounded_size (TYPE), \
  *((TYPE *) (AP - __kva_rounded_size (TYPE))))

//#endif


/* vsprintf() - dissolve conversions */
void kvsprintf(char *str, const char *format, kva_list ap)
{
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
					c = (char) kva_arg(ap, int);
					i = strlen(p);
					p[i] = c;
					p[i + 1]  = '\0';
					break;		
				case 's':	/* string */
					s = kva_arg(ap, char *);
					strcat(p, s);
					break;
				case 'd':	/* integer */
				case 'i':
					d = kva_arg(ap, int);
					itoa(d, buf, 10);	/* convert to string */
					strcat(p, buf);
					break;
				case 'x':	/* hexa-decimal */
					d = kva_arg(ap, int);
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

/* sprintf() 
 * prints format to str
 */
int sprintf (char *str, const char *format, ...) {
		kva_list ap;

		kva_start(ap, format);
		kvsprintf(str, format, ap);
		kva_end(ap);

		return strlen(str);
}


