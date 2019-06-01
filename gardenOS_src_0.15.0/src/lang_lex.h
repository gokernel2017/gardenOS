//-------------------------------------------------------------------
//
// SUMMER LANGUAGE:
//
// The Lexical Analyzer
//
// FILE:
//   lex.h
//
// SUMMER LANGUAGE START DATE ( 27/08/2017 - 08:35 ):
//   rewrite: 23/03/2019 - 08:50
//
// BY: Francisco - gokernel@hotmail.com
//
//-------------------------------------------------------------------
//
#ifndef _LEX_H_
#define _LEX_H_

#ifdef __cplusplus
extern "C" {
#endif

//-----------------------------------------------
//------------------  INCLUDE  ------------------
//-----------------------------------------------
//
//#include <stdio.h>
//#include <string.h>
//#include <ctype.h>   // isdigit()

//-----------------------------------------------
//---------------  DEFINE / ENUM  ---------------
//-----------------------------------------------
//
#define LEXER_NAME_SIZE   1024
#define LEXER_TOKEN_SIZE  1024 * 4

enum {
    TOK_INT = 255,
    TOK_FLOAT,
    TOK_VAR,
    TOK_ASM,
    TOK_IF,
    TOK_FOR,
    TOK_BREAK,
    TOK_FUNCTION,
    TOK_INCLUDE,
    TOK_MODULE,
    TOK_IMPORT,
    TOK_DEFINE,
    TOK_IFDEF,
    TOK_ENDIF,
    //
    TOK_ID,
    TOK_STRING,
    TOK_NUMBER,
    //
    TOK_PLUS_PLUS,    // ++
    TOK_MINUS_MINUS,  // --
    TOK_EQUAL_EQUAL,  // ==
    TOK_AND_AND,      // &&
    TOK_OR_OR,        // ||
    TOK_PLUS_EQUAL,   // +=
    TOK_MINUS_EQUAL,  // -=
    TOK_NOT_EQUAL     // !=
};

//-----------------------------------------------
//-------------------  STRUCT  ------------------
//-----------------------------------------------
//
typedef struct LEXER LEXER;

struct LEXER {
    char  *text;
    char  name [LEXER_NAME_SIZE + 1];
    char  token [LEXER_TOKEN_SIZE + 1];
    int   tok;
    int   pos;
    int   line;
    int   level; // in: '{' := level++ | in: '}' := level--;
    int   ifdef_block;
};

//-----------------------------------------------
//--------------  GLOBAL VARIABLE  --------------
//-----------------------------------------------
//

//-----------------------------------------------
//-----------------  PUBLIC API  ----------------
//-----------------------------------------------
//
extern int  lex         (LEXER *l);
extern int  lex_set     (LEXER *l, char *text, char *name);
extern void lex_save    (LEXER *l);
extern void lex_restore (LEXER *l);

// ERRO:
extern void   Erro      (char *format, ...);
extern char	*	ErroGet   (void);
extern void   ErroReset (void);

#ifdef __cplusplus
}
#endif
#endif // ! _LEX_H_

