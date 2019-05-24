/* -*- Mode: C; c-basic-offset: 3 -*-
 *
 * intr.h - Interrupt vector management and interrupt routing.
 *
 * This file is part of Metalkit, a simple collection of modules for
 * writing software that runs on the bare metal. Get the latest code
 * at http://svn.navi.cx/misc/trunk/metalkit/
 *
 * Copyright (c) 2008-2009 Micah Dowty
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _INTR_H_
#define _INTR_H_

#define TRUE            1
#define FALSE           0

#define IRQ_TIMER       0
#define IRQ_KEYBOARD    1

#define arraysize(var)          (sizeof(var) / sizeof((var)[0]))

#define PACKED       __attribute__ ((__packed__))
#define ALIGNED(n)   __attribute__ ((aligned(n)))
#define fastcall     __attribute__ ((fastcall))

#define PIT_HZ              1193182

// boot.h
#define BOOT_CODE_SEG       0x08
#define BOOT_LDT_SEG        0x28

#define NUM_INTR_VECTORS    256
#define NUM_FAULT_VECTORS   0x20
#define NUM_IRQ_VECTORS     0x10
#define IRQ_VECTOR_BASE     NUM_FAULT_VECTORS
#define IRQ_VECTOR(irq)     ((irq) + IRQ_VECTOR_BASE)
#define USER_VECTOR_BASE    (IRQ_VECTOR_BASE + NUM_IRQ_VECTORS)
#define USER_VECTOR(n)      ((n) + USER_VECTOR_BASE)

#define FAULT_DE            0x00    // Divide error
#define FAULT_NMI           0x02    // Non-maskable interrupt
#define FAULT_BP            0x03    // Breakpoint
#define FAULT_OF            0x04    // Overflow
#define FAULT_BR            0x05    // Bound range
#define FAULT_UD            0x06    // Undefined opcode
#define FAULT_NM            0x07    // No FPU
#define FAULT_DF            0x08    // Double Fault
#define FAULT_TS            0x0A    // Invalid TSS
#define FAULT_NP            0x0B    // Segment not present
#define FAULT_SS            0x0C    // Stack-segment fault
#define FAULT_GP            0x0D    // General Protection Fault
#define FAULT_PF            0x0E    // Page fault
#define FAULT_MF            0x10    // Math fault
#define FAULT_AC            0x11    // Alignment check
#define FAULT_MC            0x12    // Machine check
#define FAULT_XM            0x13    // SIMD floating point exception

#define PIC1_COMMAND_PORT  0x20
#define PIC1_DATA_PORT     0x21
#define PIC2_COMMAND_PORT  0xA0
#define PIC2_DATA_PORT     0xA1

#define _ASM __asm__ __volatile__

typedef void (*IntrHandler) (int vector);

fastcall void intr_Init(void);

void intr_SetMask (int irq, char enable);

void intr_SetHandler (int vector, IntrHandler handler);

void intr_timer_InitPIT (unsigned short divisor);

void intr_Disable (void);

void Intr_Enable(void);

#endif /* _INTR_H_ */
