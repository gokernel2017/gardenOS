
//-----------------------------------------------
//---------------  DEFINE / ENUM  ---------------
//-----------------------------------------------
//

#define GVAR_SIZE                 255

enum {
    TYPE_LONG = 0,
    TYPE_FLOAT,
    TYPE_FLOATP,  // float *fp;
    TYPE_STRING,
    TYPE_POINTER,
    TYPE_STRUCT,
    TYPE_PSTRUCT,  // struct data *p;
    TYPE_UNKNOW
};

enum {
    FUNC_TYPE_NATIVE_C = 0,
    FUNC_TYPE_COMPILED,
    FUNC_TYPE_VM,
    FUNC_TYPE_MODULE     // .dll | .so
};

typedef union   VALUE     VALUE;
typedef struct  TVar      TVar;
typedef struct  TFunc     TFunc;

union VALUE {
    long    l;  //: type integer
    float   f;  //: type float
    char    *s; //: type pointer of char
    void    *p; //: type pointer
};
struct TVar {
    char    *name;
    int     type;
    VALUE   value;
    void    *info;  // any information ... struct type use this
};
struct TFunc {
    char    *name;
    char    *proto; // prototype
    UCHAR   *code;  // the function on JIT MODE | or VM in VM MODE
    int     type;   // FUNC_TYPE_NATIVE_C = 0, FUNC_TYPE_COMPILED, FUNC_TYPE_VM
    int     len;
    int     sub_esp;  // used only in dynamic modules: .dll | .so
    TFunc   *next;
};


extern TVar Gvar [GVAR_SIZE + 1];

//
// SUMMER LANGUAGE | lang_core.c
//
extern ASM    * core_Init       (unsigned int size);
extern int      core_Parse      (LEXER *l, ASM *a, char *text, char *name);

extern void     CreateVarLong   (char *name, long l);
extern TFunc  * FuncFind        (char *name);
extern int      VarFind         (char *name);


extern void lib_info (int arg);

