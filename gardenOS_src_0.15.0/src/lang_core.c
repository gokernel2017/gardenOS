//-------------------------------------------------------------------
//
// SUMMER LANGUAGE:
//
// Main Core:
//
// FILE:
//   lang_core.c
//
// SUMMER LANGUAGE START DATE ( 27/08/2017 - 08:35 ):
//
// BY: Francisco - gokernel@hotmail.com
//
//-------------------------------------------------------------------
//
#include "garden.h"

//-----------------------------------------------
//-----------------  PROTOTYPES  ----------------
//-----------------------------------------------
//
static void   word_int      (LEXER *l, ASM *a);

static void   expression    (LEXER *l, ASM *a);
static int    expr0         (LEXER *l, ASM *a);
static void   expr1         (LEXER *l, ASM *a);
static void   expr2         (LEXER *l, ASM *a);
static void   expr3         (LEXER *l, ASM *a);
static void   atom          (LEXER *l, ASM *a);

static int    stmt          (LEXER *l, ASM *a);
static int    see           (LEXER *l);
static void   execute_call  (LEXER *l, ASM *a, TFunc *func);
//
void lib_info (int arg);
void lib_printi (int i);

//-----------------------------------------------
//------------------  VARIABLES  ----------------
//-----------------------------------------------
//
TVar Gvar [GVAR_SIZE + 1];

static int
    is_function,
    main_variable_type,
    var_type
    ;

static TFunc stdlib[]={
  //-----------------------------------------------------------------------------
  // char*        char*     UCHAR*                    int   int   int       FUNC*
  // name         proto     code                      type  len   sub_esp   next
  //-----------------------------------------------------------------------------
  { "info",       "0i",     (UCHAR*)lib_info,         0,    0,    0,        NULL },
  { "printi",     "0i",     (UCHAR*)lib_printi,       0,    0,    0,        NULL },
  { NULL, NULL, NULL, 0, 0, 0, NULL }
};


ASM * core_Init (unsigned int size) {
    ASM *a;
    if ((a = asm_New (size)) == NULL) {
        video_puts ("Sorry Language Initialized | core_Init Failled\n");
        return NULL;
    }
    return a;
}

static int expr0 (LEXER *l, ASM *a) {
    if (l->tok == TOK_ID) {
        //---------------------------------------
        //
        // Expression type:
        //
        //   i = a * b + c;
        //
        //---------------------------------------
        if (see(l)=='=') {
            int i;
            if ((i = VarFind(l->token)) != -1) {
                lex_save (l); // save the lexer position
                if (lex(l)=='=') {
                    lex(l);
                    expr1(l,a);
                    if (Gvar[i].type==TYPE_FLOAT) {
//                        emit_float_fstps (a, &Gvar[i].value.f);
                    } else {
//                        emit_mov_reg_var (a, EAX, &Gvar[i].value.l);
                    }
                    return i;
                }
                else lex_restore (l); // restore the lexer position
            }// if ((i = VarFind(l->token)) != -1)
        }// if (see(l)=='=')
    }
    expr1(l, a);
    return -1;
}// expr0 ()

static void expr1 (LEXER *l, ASM *a) { // '+' '-' : ADDITION | SUBTRACTION
    int op;
    expr2(l, a);
    while ((op=l->tok) == '+' || op == '-') {
        lex(l);
        expr2(l,a);
        if (main_variable_type==TYPE_FLOAT) {
//            if (op=='+') emit_add_float(a);
//            if (op=='-') emit_sub_float(a);
        } else { // TYPE_LONG
            if (op=='+') emit_expression_add_long(a);
            if (op=='-') emit_expression_sub_long(a);
        }
    }
}// expr1 ()

static void expr2 (LEXER *l, ASM *a) { // '*' '/' : MULTIPLICATION | DIVISION
    int op;
    expr3(l,a);
    while ((op=l->tok) == '*' || op == '/') {
        lex(l);
        expr3(l, a);
        if (main_variable_type==TYPE_FLOAT) {
//            if (op=='*') emit_mul_float(a);
//            if (op=='/') emit_div_float(a);
        } else { // TYPE_LONG
            if (op=='*') emit_expression_mul_long(a);
            if (op=='/') emit_expression_div_long(a);
        }
    }
}// expr2 ()

static void expr3 (LEXER *l, ASM *a) { // '('
    if (l->tok=='(') {
        lex(l);
        expr0(l,a);
        if (l->tok != ')') {
            Erro("ERRO )\n");
        }
        lex(l);
    }
    else atom(l,a);
}// expr3 ()

static void atom (LEXER *l, ASM *a) { // expres
    if (l->tok == TOK_ID) {
        int i;
        if ((i = VarFind(l->token)) != -1) {
            var_type = Gvar[i].type;

            if (main_variable_type==TYPE_FLOAT && var_type != TYPE_FLOAT) {
            #ifdef __x86_64__
                // db 04 25     10 30 40 00 	fildl  0x403010
                g3(a,0xdb,0x04,0x25); asm_get_addr(a, &Gvar[i].value.l);
            #else
                // db 05    70 40 40 00    	fildl  0x404070
                g2(a,0xdb,0x05); asm_get_addr(a, &Gvar[i].value.l);
            #endif
            } else {
                if (var_type==TYPE_FLOAT) {
//                    emit_float_flds (a, &Gvar[i].value.f);
                } else {
                    emit_expression_push_var (a, &Gvar[i].value.l);
                }
            }

            lex(l);
        }
        else Erro ("%s: %d: - Ilegal Word '%s'\n", l->name, l->line, l->token);
    }
    else if (l->tok==TOK_NUMBER) {
        if (strchr(l->token, '.'))
            var_type = TYPE_FLOAT;

        if (var_type==TYPE_FLOAT) {
//            emit_push_float(a, atof(l->token));
        } else {
            emit_expression_push_long (a, atoi(l->token));
        }
        lex(l);
    }
    else {
        printk ("ATOM ERRO TOKEN(%s) ... SAINDO\n", l->token);
        Erro("%s: %d Expression atom - Ilegal Word (%s)\n", l->line, l->token);
    }
}// atom ()


void expression (LEXER *l, ASM *a) {
    if (l->tok==TOK_ID || l->tok==TOK_NUMBER) {
        TFunc *fi;

        // call a function without return:
        //   function_name (...);
        //
        if ((fi = FuncFind(l->token)) != NULL) {
            execute_call (l, a, fi);
            return;
        }
    }
}

static void do_block (LEXER *l, ASM *a) {
    while (!erro && l->tok && l->tok != '}') {
        stmt (l,a);
    }
    l->tok = ';';
}

static int stmt (LEXER *l, ASM *a) {
    lex (l);
    switch (l->tok) {
    case '{':
        l->level++;
        //----------------------------------------------------
        do_block (l,a); //<<<<<<<<<<  no recursive  >>>>>>>>>>
        //----------------------------------------------------
        return 1;
    case TOK_INT: word_int (l,a); return 1;
//    case TOK_VAR: word_var (l,a); return 1;
    default: expression (l,a); return 1;
    case '}': l->level--; return 1;
    case '#':
    case ';':
    case ',':
        return 1;
    case 0: return 0;
    }
    return 1;
}

int core_Parse (LEXER *l, ASM *a, char *text, char *name) {
    lex_set (l, text, name);
    ErroReset ();
    asm_Reset (a);
    emit_begin (a);
    while (!erro && stmt(l,a)) {
        // ... compiling ...
    }
    emit_end (a);
    if (l->level) // { ... }
        Erro ("\nERRO: LEXER->level { ... }: %d\n", l->level);
    if (l->ifdef_block)
        Erro ("%s: #ifdef block need close ...\n", l->name);
    return erro;
}

void CreateVarLong (char *name, long l) {
    TVar *v = Gvar;
    int i = 0;
    while (v->name) {
        if (!strcmp(v->name, name))
            return;
        i++;
        v++;
    }
    if (i < GVAR_SIZE) {
        v->name = strdup(name);
        v->type = TYPE_LONG;
        v->value.l = l;
        v->info = NULL;
    }
}

TFunc *FuncFind (char *name) {
    // array:
    TFunc *lib = stdlib;
    while (lib->name) {
        if ((lib->name[0]==name[0]) && !strcmp(lib->name, name))
            return lib;
        lib++;
    }
/*
  // linked list:
  TFunc *func = Gfunc;
  while (func) {
    if ((func->name[0]==name[0]) && !strcmp(func->name, name))
      return func;
    func = func->next;
  }
*/
    return NULL;
}

int VarFind (char *name) {
    TVar *v = Gvar;
    int i = 0;
    while (v->name) {
        if (!strcmp(v->name, name))
            return i;
        i++;
        v++;
    }
    return -1;
}

static int see (LEXER *l) {
    char *s = l->text+l->pos;
    while (*s) {
        if (*s=='\n' || *s==' ' || *s==9 || *s==13) {
        s++;
        } else {
            if (s[0]=='=' && s[1]=='=') return TOK_EQUAL_EQUAL;
            if (s[0]=='+' && s[1]=='+') return TOK_PLUS_PLUS;
            if (s[0]=='+' && s[1]=='=') return TOK_PLUS_EQUAL;
            if (s[0]=='-' && s[1]=='-') return TOK_MINUS_MINUS;
            return *s;
        }
    }
    return 0;
}

//
// function_name (a, b, c + d);
//
static void execute_call (LEXER *l, ASM *a, TFunc *func) {
    int count = 0, pos = 0, size = 4;

    while (lex(l)) {
        if (l->tok==TOK_ID || l->tok==TOK_NUMBER || l->tok==TOK_STRING || l->tok=='(') {

            main_variable_type = var_type = TYPE_LONG;

            asm_expression_reset(); // reg = 0;
            // The result of expression is store in the register: %eax.
            expr0 (l,a);

            emit_mov_eax_ESP(a, pos);
            size = 4;
            pos += size;

            if (count++ > 15) break;
        }
        if (l->tok == ')' || l->tok == ';') break;
    }
    emit_call (a, func->code);
}

static void word_int (LEXER *l, ASM *a) {
    while (lex(l)) {
        if (l->tok==TOK_ID) {
            char name[255];
            int value = 0;
            strcpy (name, l->token); // save
            if (lex(l)=='=') {
                if (lex(l)==TOK_NUMBER)
                    value = atoi (l->token);
            }
            if (is_function) {
/*
                //---------------------------------------------------
                // this is temporary ...
                // in function(word_function) changes ...
                //---------------------------------------------------
                if (local_count < LOCAL_MAX) {
                    sprintf (local[local_count].name, "%s", name);
                    local[local_count].type = TYPE_LONG;
                    local[local_count].value.l = value;
                    local[local_count].info = NULL;
                    local_count++;
                }
                else Erro ("Variable Local Max %d\n", LOCAL_MAX);
                // ... need implementation ...
*/
            }
            else CreateVarLong (name, value);
        }
        if (l->tok == ';') break;
    }
    if (l->tok != ';') Erro ("ERRO: The word(int) need the char(;) on the end\n");
}// word_int()


void lib_info (int arg) {
    switch (arg) {
    case 1: {
        TVar *v = Gvar;
        int i = 0;
        printk ("VARIABLES:\n---------------\n");
        while (v->name) {
            if (v->type==TYPE_LONG)   printk ("Gvar[%d](%s) = %d\n", i, v->name, v->value.l);
            else
            if (v->type==TYPE_FLOAT) printk ("Gvar[%d](%s) = %f\n", i, v->name, v->value.f);
            v++; i++;
        }
        } break;
/*
    case 2: {
        TFunc *fi = stdlib;
        printf ("FUNCTIONS:\n---------------\n");
        while (fi->name) {
            if(fi->proto){
                char *s=fi->proto;
                if (*s=='0') printf ("void   ");
                else
                if (*s=='i') printf ("int    ");
                else
                if (*s=='f') printf ("float  ");
                else
                if (*s=='s') printf ("char   *");
                else
                if (*s=='p') printf ("void * ");
                printf ("%s (", fi->name);
                s++;
                while(*s){
                    if (*s=='0') printf ("void");
										else
                    if (*s=='i') printf ("int");
                    else
                    if (*s=='f') printf ("float");
                    else
                    if (*s=='s') printf ("char *");
                    else
                    if (*s=='p') printf ("void *");
                    else
                    if (*s=='.') printf ("...");
                    s++;
                    if(*s) printf (", ");
                }
                printf (");\n");
            }
            fi++;
        }
        fi = Gfunc;
        while(fi){
            if(fi->proto){
                char *s=fi->proto;
                if (*s=='0') printf ("void  ");
                else
                if (*s=='i') printf ("int   ");
                else
                if (*s=='f') printf ("float ");
                else
                if (*s=='s') printf ("char  *");
                else
                if (*s=='p') printf ("void * ");
                printf ("%s (", fi->name);
                s++;
                while(*s){
                    if (*s=='0') printf ("void");
										else
                    if (*s=='i') printf ("int");
                    else
                    if (*s=='f') printf ("float");
                    else
                    if (*s=='s') printf ("char *");
                    else
                    if (*s=='p') printf ("void *");
                    else
                    if (*s=='.') printf ("...");
                    s++;
                    if(*s) printf (", ");
                }
                printf (");\n");
            }
            fi = fi->next;
        }
        TModule *p = Gmodule;
        while (p) {
            TFunc *f = p->func;
            printf ("MODULE: '%s'\n", p->name);
            while (f) {
                printf ("  '%s'\n", f->name);
                f = f->next;
            }
            p = p->next;
        }

				}
        break;

    case 3: {
        TDefine *o = Gdefine;
        printf ("ALL DEFINES:\n---------------\n");
        while (o) {
            printf ("  %s : %d\n", o->name, o->value);
            o = o->next;
        }
        } break;
*/
    default:
        printk ("USAGE(%d): info(1);\n\nInfo Options:\n 1: Variables\n 2: Functions\n 3: Defines\n 4: Words\n",arg);
    }
}

void lib_printi (int i) {
    printk ("Result: %d\n", i);
}

