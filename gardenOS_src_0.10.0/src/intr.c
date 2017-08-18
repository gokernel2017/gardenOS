/* -*- Mode: C; c-basic-offset: 3 -*-
 *
 * intr.c - Interrupt vector management, interrupt routing,
 *          and low-level building blocks for multithreading.
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

#include "intr.h"

extern unsigned char inb  (unsigned short port);
extern void          outb (unsigned short port, unsigned char value);

/*
 * To save space, we don't include assembly-language trampolines for
 * each interrupt vector. Instead, we allocate a table in the BSS
 * segment which we can fill in at runtime with simple trampoline
 * functions. This structure actually describes executable 32-bit
 * code.
 */

typedef struct {
   unsigned short code1;
   unsigned int   arg;
   unsigned char  code2;
   IntrHandler    handler;
   unsigned int   code3;
   unsigned int   code4;
   unsigned int   code5;
   unsigned int   code6;
   unsigned int   code7;
   unsigned int   code8;
} PACKED IntrTrampolineType;
//} IntrTrampolineType;



/*
 * IDT table and IDT table descriptor. The table itself lives in the
 * BSS segment, the descriptor lives in the data segment.
 */

typedef union {
   struct {
      unsigned short offsetLow;
      unsigned short segment;
      unsigned short flags;
      unsigned short offsetHigh;
   };
   struct {
      unsigned int offsetLowSeg;
      unsigned int flagsOffsetHigh;
   };
} __attribute__ ((__packed__)) IDTType;

/*
 * Note the IDT is page-aligned. Only 8-byte alignment is actually
 * necessary, though page alignment may help performance in some
 * environments.
 */
//static IDTType ALIGNED(4096) IDT[NUM_INTR_VECTORS];
static IDTType IDT[NUM_INTR_VECTORS];

const struct {
   unsigned short limit;
   void *address;
} __attribute__ ((__packed__)) IDTDesc = {
   .limit = NUM_INTR_VECTORS * 8 - 1,
   .address = IDT,
};

IntrTrampolineType ALIGNED(4) IntrTrampoline[NUM_INTR_VECTORS];


/*
 * Intr_SetHandler --
 *
 *    Set a C-language interrupt handler for a particular vector.
 *    Note that the argument is a vector number, not an IRQ.
 */

void intr_SetHandler (int vector, IntrHandler handler) {
    IntrTrampoline[vector].handler = handler;
}

/*
 * Intr_SetMask --
 *
 *    (Un)mask a particular IRQ.
 */

void intr_SetMask (int irq, char enable) {
    unsigned char port, bit, mask;

   if (irq >= 8) {
      bit = 1 << (irq - 8);
      port = PIC2_DATA_PORT;
   } else {
      bit = 1 << irq;
      port = PIC1_DATA_PORT;
   }

   mask = inb(port);

   /* A '1' bit in the mask inhibits the interrupt. */
   if (enable) {
      mask &= ~bit;
   } else {
      mask |= bit;
   }

   outb (port, mask);
}



void intr_Halt(void) {
   //asm volatile ("hlt");
   asm ("hlt");
}

void function_null (int i) {

}


/*
 * IntrDefaultHandler --
 *
 *    Default no-op interrupt handler.
 */

void IntrDefaultHandler (int vector)
{
   /* Do nothing. */
}

void Intr_Enable(void) {
   asm volatile ("sti");
}

void intr_Disable (void) {
   asm volatile ("cli");
}


void Intr_Break (void) {
   asm volatile ("int3");
}


/*
 * Intr_Init --
 *
 *    Initialize the interrupt descriptor table and the programmable
 *    interrupt controller (PIC). On return, interrupts are enabled
 *    but all handlers are no-ops.
 */

fastcall void intr_Init (void) {
   int i;

   intr_Disable();

   IDTType *idt = IDT;
   IntrTrampolineType *tramp = IntrTrampoline;

   for (i = 0; i < NUM_INTR_VECTORS; i++) {
      unsigned int trampolineAddr = (unsigned int) tramp;

      /*
       * Set up the IDT entry as a 32-bit interrupt gate, pointing at
       * our trampoline for this vector. Fill in the IDT with two 32-bit
       * writes, since GCC generates significantly smaller code for this
       * than when writing four 16-bit fields separately.
       */

      idt->offsetLowSeg = (trampolineAddr & 0x0000FFFF) | (BOOT_CODE_SEG << 16);
      idt->flagsOffsetHigh = (trampolineAddr & 0xFFFF0000) | 0x00008E00;

      /*
       * Set up the trampoline, pointing it at the default handler.
       * The trampoline function wraps our C interrupt handler, and
       * handles placing a vector number onto the stack. It also allows
       * interrupt handlers to switch stacks upon return by writing
       * to the saved 'esp' register.
       *
       * Note that the old stack and new stack may actually be different
       * stack frames on the same stack. We require that the new stack
       * is in a higher or equal stack frame, but the two stacks may
       * overlap. This is why the trampoline does its copy in reverse.
       *
       * Keep the trampoline function consistent with the definition
       * of IntrContext in intr.h.
       *
       * Stack layout:
       *
       *     8   eflags
       *     4   cs
       *     0   eip        <- esp on entry to IRQ handler
       *    -4   eax
       *    -8   ecx
       *   -12   edx
       *   -16   ebx
       *   -20   esp
       *   -24   ebp
       *   -28   esi
       *   -32   edi
       *   -36   <arg>      <- esp on entry to handler function
       *
       * Our trampolines each look like:
       *
       *    60                 pusha                   // Save general-purpose regs
       *    68 <32-bit arg>    push   <arg>            // Call handler(arg)
       *    b8 <32-bit addr>   mov    <addr>, %eax
       *    ff d0              call   *%eax
       *    58                 pop    %eax             // Remove arg from stack
       *    8b 7c 24 0c        mov    12(%esp), %edi   // Load new stack address
       *    8d 74 24 28        lea    40(%esp), %esi   // Addr of eflags on old stack
       *    83 c7 08           add    $8, %edi         // Addr of eflags on new stack
       *    fd                 std                     // Copy backwards
       *    a5                 movsl                   // Copy eflags
       *    a5                 movsl                   // Copy cs
       *    a5                 movsl                   // Copy eip
       *    61                 popa                    // Restore general-purpose regs
       *    8b 64 24 ec        mov    -20(%esp), %esp  // Switch stacks
       *    cf                 iret                    // Restore eip, cs, eflags
       *
       * Note: Surprisingly enough, it's actually more size-efficient to initialize
       * the structure in code like this than it is to memcpy() the trampoline from
       * a template in the data segment.
       */

      tramp->code1 = 0x6860;
      tramp->code2 = 0xb8;
      tramp->code3 = 0x8b58d0ff;
      tramp->code4 = 0x8d0c247c;
      tramp->code5 = 0x83282474;
      tramp->code6 = 0xa5fd08c7;
      tramp->code7 = 0x8b61a5a5;
      tramp->code8 = 0xcfec2464;

      tramp->handler = IntrDefaultHandler;
      tramp->arg = i;

      idt++;
      tramp++;
   }

#ifdef WIN32
   asm volatile ("lidt _IDTDesc");
#endif
#ifdef __linux__
   asm volatile ("lidt IDTDesc");
#endif

   typedef struct {
      unsigned char port, data;
   } PortData8;

   static const PortData8 pitInit[] = {
      /*
       * Program the PIT to map all IRQs linearly starting at
       * IRQ_VECTOR_BASE.
       */

      { PIC1_COMMAND_PORT, 0x11 },       // Begin init, use 4 command words
      { PIC2_COMMAND_PORT, 0x11 },
      { PIC1_DATA_PORT, IRQ_VECTOR_BASE },
      { PIC2_DATA_PORT, IRQ_VECTOR_BASE + 8 },
      { PIC1_DATA_PORT, 0x04 },
      { PIC2_DATA_PORT, 0x02 },
      { PIC1_DATA_PORT, 0x03 },          // 8086 mode, auto-end-of-interrupt.
      { PIC2_DATA_PORT, 0x03 },

      /*
       * All IRQs start out masked, except for the cascade IRQs 2 and 4.
       */
      { PIC1_DATA_PORT, 0xEB },
      { PIC2_DATA_PORT, 0xFF },
   };

    const PortData8 *p = pitInit;

    for (i = arraysize(pitInit); i; i--, p++) {
        outb (p->port, p->data);
    }

    Intr_Enable();

    for (i = 0; i < NUM_FAULT_VECTORS; i++) {
        IntrTrampoline [ i ].handler = function_null;
    }
}

//fastcall void Timer_InitPIT (uint16 divisor) {
void intr_timer_InitPIT (unsigned short divisor) {
    outb (0x43, 0x34);
    outb (0x40, divisor & 0xFF);
    outb (0x40, divisor >> 8);
}
