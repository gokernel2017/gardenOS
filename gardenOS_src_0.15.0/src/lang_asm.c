//-------------------------------------------------------------------
//
// MINI LANGUAGE:
//
// The Backend | JIT(x86) 32/64 bits:
//
// FILE:
//   asm.c
//
// BY: Francisco - gokernel@hotmail.com
//
//-------------------------------------------------------------------
//
#include "garden.h"

#ifdef USE_SUMMER_LANGUAGE

#define REG_MAX 6

typedef struct ASM_label  ASM_label;
typedef struct ASM_jump   ASM_jump;

struct ASM { // opaque struct
    UCHAR     *p;
    UCHAR     *code;
    ASM_label *label;
    ASM_jump  *jump;
    int       size;
};
struct ASM_label {
    char      *name;
    int       pos;
    ASM_label *next;
};
struct ASM_jump {
    char      *name;
    int       pos;
    int       type;
    int       exist; // to check if this 'exist'
    ASM_jump  *next;
};

//
// const EMIT opcode:
//
const UCHAR OP_mov_eax_ecx  [2] = { 0x89, 0xc1 }; // mov	%eax, %ecx
const UCHAR OP_add_ecx_eax  [2] = { 0x01, 0xc8 }; // add %ecx, %eax
const UCHAR OP_imul_ecx_eax [3] = { 0x0f, 0xaf, 0xc1 }; // imul %ecx, %eax
const UCHAR OP_imul_edx_ecx [3] = { 0x0f, 0xaf, 0xca }; // imul %edx, %ecx
const UCHAR OP_sub_ecx_eax [2] = { 0x29, 0xc8 }; // sub %ecx, %eax
const UCHAR OP_sub_edx_ecx [2] = { 0x29, 0xd1 }; // sub %edx, %ecx


//static char *REGISTER[REG_MAX] = { "%eax", "%ecx", "%edx", "%ebx", "%esi", "%edi" };
// used for expression in register:
static int	reg; // EAX, ECX, EDX, EBX, ESI, EDI

static void push_register (void) { if (reg < REG_MAX) reg++; }
static void pop_register (void)  { if (reg > 0) reg--; }

//-------------------------------------------------------------------
//-------------------------  ASM PUCLIC API  ------------------------
//-------------------------------------------------------------------
//
ASM *asm_New (unsigned int size) {
    ASM *a = (ASM*)kmalloc(sizeof(ASM));
    if (a && (a->code=(UCHAR*)kmalloc(size)) != NULL) {
        a->p     = a->code;
        a->label = NULL;
        a->jump  = NULL;
        a->size  = size;
        return a;
    }
    return NULL;
}

void asm_Run (ASM *a) {
    ( (void(*)()) a->code ) ();
}

void asm_Reset (ASM *a) {

    a->p = a->code;

    // reset ASM_label:
    while (a->label != NULL) {
        ASM_label *temp = a->label->next;
        if (a->label->name) {
            printk("FREE LABEL TEXT(%s)\n", a->label->name);
            kfree (a->label->name);
        }
        kfree (a->label);
        a->label = temp;
    }
    // reset ASM_jump:
    while (a->jump != NULL) {
        ASM_jump *temp = a->jump->next;
        if (a->jump->name)
            kfree(a->jump->name);
        kfree (a->jump);
        a->jump = temp;
    }

    a->label = NULL;
    a->jump  = NULL;
}

void asm_get_addr (ASM *a, void *ptr) { ///: 32/64 BITS OK
    *(void**)a->p = ptr;
    //a->p += sizeof(void*);
    a->p += 4; // ! OK
}

void asm_Label (ASM *a, char *name) {
    if (name) {
        ASM_label *lab;
        ASM_label *l = a->label;

        // find if exist:
        while (l) {
            if (!strcmp(l->name, name)) {
                printk ("Label Exist: '%s'\n", l->name);
                return;
            }
            l = l->next;
        }

        if ((lab = (ASM_label*)kmalloc (sizeof(ASM_label))) != NULL) {

            lab->name = strdup (name);
            lab->pos  = (a->p - a->code); // the index

            // add on top:
            lab->next = a->label;
            a->label = lab;
        }
    }
}

//-------------------------------------------------------------------
//---------------------------  GEN / EMIT  --------------------------
//-------------------------------------------------------------------
//
void g (ASM *a, UCHAR c) {
    *a->p++ = c;
}
void gen (ASM *a, UCHAR c) {
    *a->p++ = c;
}
void g2 (ASM *a, UCHAR c1, UCHAR c2) {
    a->p[0] = c1;
    a->p[1] = c2;
    a->p += 2;
}
void g3 (ASM *a, UCHAR c1, UCHAR c2, UCHAR c3) {
    a->p[0] = c1;
    a->p[1] = c2;
    a->p[2] = c3;
    a->p += 3;
}
void g4 (ASM *a, UCHAR c1, UCHAR c2, UCHAR c3, UCHAR c4) {
    a->p[0] = c1;
    a->p[1] = c2;
    a->p[2] = c3;
    a->p[3] = c4;
    a->p += 4;
}
void g5 (ASM *a, UCHAR c1, UCHAR c2, UCHAR c3, UCHAR c4, UCHAR c5) {
    a->p[0] = c1;
    a->p[1] = c2;
    a->p[2] = c3;
    a->p[3] = c4;
    a->p[4] = c5;
    a->p += 5;
}

void emit (ASM *a, const UCHAR opcode[], unsigned int len) {
    while (len--) {
        *a->p++ = *opcode++;
    }
}

void emit_begin (ASM *a) { //: 32/64 BITS OK
    #if defined(__x86_64__)
    // 55         : push  %rbp
    // 48 89 e5   : mov   %rsp,%rbp
    //-----------------------------
    a->p[0] = 0x55;
    a->p[1] = 0x48;
    a->p[2] = 0x89;
    a->p[3] = 0xe5;
    a->p += 4;
    emit_sub_esp(a,48); // 48 / 8 := 6
    #else
    // 55     : push  %ebp
    // 89 e5  : mov   %esp,%ebp
    //-----------------------------
    a->p[0] = 0x55;
    a->p[1] = 0x89;
    a->p[2] = 0xe5;
    a->p += 3;
    emit_sub_esp(a,100);
    #endif
}

void emit_end (ASM *a) { ///: 32/64 BITS OK
    #if defined(__x86_64__)
    a->p[0] = 0xc9; // c9 : leaveq
    a->p[1] = 0xc3; // c3 : retq
    a->p += 2;
    #else
    a->p[0] = 0xc9; // c9 : leave
    a->p[1] = 0xc3; // c3 : ret
    a->p += 2;
    #endif
    if ((a->p - a->code) > a->size) {
//        Erro ("ASM ERRO: code > size\n");
        return;
    }
//    asm_change_jump (a);
}

void emit_sub_esp (ASM *a, char c) { // 32/64 BITS OK
    #if defined(__x86_64__)
    g4(a,0x48,0x83,0xec,(char)c); // 48 83 ec   08   sub   $0x8,%rsp
    #else
    g3(a,0x83,0xec,(char)c);      // 83 ec      08   sub  $0x8,%esp
    #endif
}

void emit_incl (ASM *a, void *var) { //: 32/64 BITS OK
  #if defined(__x86_64__)
  g3(a,0xff,0x04,0x25); asm_get_addr(a,var);  // ff 04 25   00 0a 60 00   : incl   0x600a00
  #else
  g2(a,0xff,0x05); asm_get_addr(a,var);       // ff 05      00 20 40 00   : incl   0x402000
  #endif
}

void emit_decl (ASM *a, void *var) { //: 32/64 BITS OK
    #if defined(__x86_64__)
    g3(a,0xff,0x0c,0x25); asm_get_addr(a, var);  // ff 0c 25   cc 0a 60 00   decl  0x600acc
    #else
    g2(a,0xff,0x0d); asm_get_addr(a, var);  // ff 0d      00 20 40 00   decl  0x402000
    #endif
}

void emit_call (ASM *a, void *func) {
#ifdef __x86_64__
    // 48 c7 c0     30 15 40 00   mov    $0x401530, %rax
    g3(a,0x48,0xc7,0xc0); asm_get_addr(a, func);
    // ff d0  |  callq  *%rax
    g2(a,0xff,0xd0);
#else
    // b8   7a 13 40 00       mov    $0x40137a, %eax
    // ff d0                	call   *%eax
    //
    g(a,0xb8); asm_get_addr(a, func);
    g2(a,0xff,0xd0);
#endif
}

// mov $0x3e8, %eax
void emit_mov_long_reg (ASM *a, long value, int reg) {
    switch (reg) {
    case EAX: g(a,0xb8); break; // b8 	e8 03 00 00		|	mov $0x3e8, %eax
    case ECX: g(a,0xb9); break; // b9 	e8 03 00 00		| mov $0x3e8, %ecx
    case EDX: g(a,0xba); break; // ba 	e8 03 00 00		| mov $0x3e8, %edx
    case EBX: g(a,0xbb); break; // bb		e8 03 00 00		| mov $0x3e8, %ebx
    case ESI: g(a,0xbe); break; // be   e8 03 00 00   | mov $0x3e8, %esi
    case EDI: g(a,0xbf); break; // bf   e8 03 00 00   | mov $0x3e8, %edi
    default: return;
    }
    *(long*)a->p = value;
    a->p += 4; // ! OK
}

void emit_mov_var_reg (ASM *a, void *var, int reg) { ///: 32/64 BITS OK: Move variable to %register
#if defined(__x86_64__)
    switch (reg) {
    case EAX: g3(a,0x8b,0x04,0x25); break; // 8b 04 25   00 0a 60 00 	mov    0x600a00,%eax
    case ECX: g3(a,0x8b,0x0c,0x25); break; // 8b 0c 25   00 0a 60 00 	mov    0x600a00,%ecx
    case EDX: g3(a,0x8b,0x14,0x25); break; // 8b 14 25   00 0a 60 00 	mov    0x600a00,%edx
    case EBX: g3(a,0x8b,0x1c,0x25); break; // 8b 1c 25   00 0a 60 00 	mov    0x600a00,%ebx
    case ESI: g3(a,0x8b,0x34,0x25); break; // 8b 34 25   20 7a 40 00  mov    0x407a20,%esi
    case EDI: g3(a,0x8b,0x3c,0x25); break; // 8b 3c 25   30 7a 40 00  mov    0x407a30,%edi
    //
    case RAX: g4(a,0x48,0x8b,0x04,0x25); break; // 48 8b 04 25    30 7a 40 00   | mov    0x407a30, %rax
    //
    // Windows function POINTER argument 1 & 2
    case RCX: g4(a,0x48,0x8b,0x0c,0x25); break; // 48 8b 0c 25    30 7a 40 00   | mov   0x407a30, %rcx
    case RDX: g4(a,0x48,0x8b,0x14,0x25); break; // 48 8b 14 25    30 7a 40 00   | mov   0x407a30, %rdx
    // Linux function POINTER argument 1 & 2
    case RDI: g4(a,0x48,0x8b,0x3c,0x25); break; // 48 8b 3c 25    30 7a 40 00   | mov   0x407a30, %rdi
    case RSI: g4(a,0x48,0x8b,0x34,0x25); break; // 48 8b 34 25    30 7a 40 00   | mov   0x407a30, %rsi
    default: return;
    }
    asm_get_addr (a, var);
#else
    switch (reg) {
    case EAX: g(a,0xa1);       break; // a1       60 40 40 00   mov   0x404060, %eax
    case ECX: g2(a,0x8b,0x0d); break;	// 8b 0d    70 40 40 00   mov   0x404070, %ecx
    case EDX: g2(a,0x8b,0x15); break; // 8b 15    70 40 40 00   mov   0x404070, %edx
    case EBX: g2(a,0x8b,0x1d); break; // 8b 1d    60 40 40 00   mov   0x404060, %ebx
    case ESI: g2(a,0x8b,0x35); break; // 8b 35 		38 54 40 00		mov   0x405438, %esi
    case EDI: g2(a,0x8b,0x3d); break; // 8b 3d 		34 54 40 00		mov   0x405434, %edi
    default: return;
    }
    asm_get_addr (a, var);
#endif
}

void emit_mov_reg_var (ASM *a, int reg, void *var) { ///: 32/64 BITS OK: Move %register to variable
    if (reg >= 0 && reg <= 7) {
        #if defined(__x86_64__)
        switch (reg) {
        case EAX: g3(a, 0x89, 0x04, 0x25);  break; // 89 04 25    28 0b 60 00 	mov  %eax, 0x600b28
        case ECX: g3(a, 0x89, 0x0c, 0x25);  break; // 89 0c 25    28 0b 60 00 	mov  %ecx, 0x600b28
        case EDX: g3(a, 0x89, 0x14, 0x25);  break; // 89 14 25    28 0b 60 00 	mov  %edx, 0x600b28
        case EBX: g3(a, 0x89, 0x1c, 0x25);  break; // 89 1c 25    28 0b 60 00 	mov  %ebx, 0x600b28
        default: return;
        }
        asm_get_addr(a,var);
        #else
        switch (reg) {
        case EAX: g(a,0xa3);       break; // a3       10 40 40 00   mov   %eax, 0x404010
        case ECX: g2(a,0x89,0x0d); break; // 89 0d    60 40 40 00   mov   %ecx, 0x404060
        case EDX: g2(a,0x89,0x15); break; // 89 15    60 40 40 00   mov   %edx, 0x404060
        case EBX: g2(a,0x89,0x1d); break;//  89 1d    60 40 40 00   mov   %ebx, 0x404060
        default: return;
        }
        asm_get_addr(a,var);
        #endif
    }
}

void emit_mov_eax_ESP (ASM *a, UCHAR index) {
    #if defined(__x86_64__)
    g5(a,0x67,0x89,0x44,0x24,(UCHAR)index); // 67 89 44 24       04    mov  %eax, 0x4(%esp)
    #else
    g4 (a,0x89,0x44,0x24,(UCHAR)index); // 89 44 24     04    mov    %eax,0x4(%esp)
    #endif
}

void emit_expression_add_long (ASM *a) {
    pop_register();
    if (reg == ECX && reg-1 == EAX) {
        EMIT(a,OP_add_ecx_eax); // add %ecx, %eax
    }
}

void emit_expression_sub_long (ASM *a) { 
    pop_register();
    if (reg == ECX && reg-1 == EAX) {
        EMIT(a,OP_sub_ecx_eax); // sub %ecx, %eax
    }
    else
    if (reg == EDX && reg-1 == ECX) {
        EMIT(a,OP_sub_edx_ecx); // sub %edx, %ecx
    }
}

void emit_expression_mul_long (ASM *a) {
    pop_register();
    if (reg == ECX && reg-1 == EAX) {
        EMIT(a,OP_imul_ecx_eax); // imul %ecx, %eax
    }
    else
    if (reg == EDX && reg-1 == ECX) {
        EMIT(a,OP_imul_edx_ecx); // imul %edx, %ecx
    }
}

void emit_expression_div_long (ASM *a) {
    pop_register();
    if (reg == ECX) {
        // DEBUG !
//        printf ("  cltd\n");
//        printf ("  idiv %s\n", REGISTER[reg]);
        // emit ...
        g(a,0x99);        // 99       cltd
        g2(a,0xf7,0xf9);  // f7 f9    idiv   %ecx
    } else {
        Erro ("asm Division erro ... please use as first expression !!!\n");
    }
}

void asm_expression_reset (void) {
    reg = 0;
}

void emit_expression_push_long (ASM *a, long value) {
    emit_mov_long_reg (a, value, reg); // mov $1000, %eax
    push_register();
}

void emit_expression_push_var (ASM *a, void *var) {
    emit_mov_var_reg (a, var, reg);
    push_register();
}

#endif // USE_SUMMER_LANGUAGE

