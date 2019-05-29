//-------------------------------------------------------------------
//
// MINI LANGUAGE:
//
// The Backend | JIT(x86) 32/64 bits:
//
// FILE:
//   asm.h
//
// BY: Francisco - gokernel@hotmail.com
//
//-------------------------------------------------------------------
//
#ifndef _ASM_H_
#define _ASM_H_

#ifdef __cplusplus
extern "C" {
#endif

//-----------------------------------------------
//---------------  DEFINE / ENUM  ---------------
//-----------------------------------------------
//
#define UCHAR               unsigned char
#define ASM_DEFAULT_SIZE    5000

enum {
    // 32 bits
    EAX = 0,
    ECX,
    EDX,
    EBX,
    ESP,
    EBP,
    ESI,
    EDI,
    // 64 bits
    RAX,
    RCX,
    RDX,
    // ...
    RSI,
    RDI
};

typedef struct ASM ASM;
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

// lang_asm.c
//
extern int erro;

//
// const EMIT opcode:
//
extern const UCHAR OP_mov_eax_ecx  [2]; // mov %eax, %ecx
extern const UCHAR OP_add_ecx_eax  [2]; // add %ecx, %eax
extern const UCHAR OP_imul_ecx_eax [3]; // imul %ecx, %eax
extern const UCHAR OP_imul_edx_ecx [3]; // imul %edx, %ecx
extern const UCHAR OP_sub_ecx_eax  [2]; // sub %ecx, %eax
extern const UCHAR OP_sub_edx_ecx  [2]; // sub %edx, %ecx


//-----------------------------------------------
//---------------  ASM PUCLIC API  --------------
//-----------------------------------------------
//
extern ASM  * asm_New           (unsigned int size);
extern void   asm_Reset         (ASM *a);
extern void   asm_Run           (ASM *a);
extern void   asm_Label         (ASM *a, char *name);

extern void   asm_get_addr      (ASM *a, void *ptr);

// ERRO:
extern void   Erro              (char *format, ...);
extern char	*	ErroGet           (void);
extern void   ErroReset         (void);

extern void   emit_call         (ASM *a, void *func);

// emit/gen:
//
#define       EMIT(a,op)				emit(a, op, sizeof(op))
extern void   emit              (ASM *a, const UCHAR opcode[], unsigned int len);
extern void   g                 (ASM *a, UCHAR c);
extern void   g2                (ASM *a, UCHAR c1, UCHAR c2);
extern void   g3                (ASM *a, UCHAR c1, UCHAR c2, UCHAR c3);
extern void   g4                (ASM *a, UCHAR c1, UCHAR c2, UCHAR c3, UCHAR c4);
extern void   g5                (ASM *a, UCHAR c1, UCHAR c2, UCHAR c3, UCHAR c4, UCHAR c5);

extern void   emit_begin        (ASM *a);
extern void   emit_end          (ASM *a);
extern void   emit_sub_esp      (ASM *a, char c);
extern void   emit_incl         (ASM *a, void *var);
extern void   emit_mov_long_reg (ASM *a, long value, int reg);
extern void   emit_mov_var_reg  (ASM *a, void *var, int reg);
extern void   emit_mov_reg_var  (ASM *a, int reg, void *var);
extern void   emit_mov_eax_ESP  (ASM *a, UCHAR index);

extern void asm_expression_reset (void);
extern void emit_expression_push_long (ASM *a, long value);
extern void emit_expression_push_var (ASM *a, void *var);

extern void emit_expression_add_long (ASM *a);
extern void emit_expression_sub_long (ASM *a);
extern void emit_expression_mul_long (ASM *a);
extern void emit_expression_div_long (ASM *a);


#ifdef __cplusplus
}
#endif
#endif // ! _ASM_H_

