//-------------------------------------------------------------------
//
// SUMMER LANGUAGE:
//
// The Lexical Analyzer
//
// FILE:
//   lex.c
//
// SUMMER LANGUAGE START DATE ( 27/08/2017 - 08:35 ):
//   rewrite 01: 20/07/2018 - 11:10
//   rewrite 02: 23/03/2019 - 08:50
//
// BY: Francisco - gokernel@hotmail.com
//
//-------------------------------------------------------------------
//
#include "garden.h"

extern void Erro (char *format, ...); // in file: "lang_asm.c"


static char save_token [LEXER_TOKEN_SIZE + 1];
static int  save_pos;
static int  save_tok;
static int  save_line;

int isdigit (int i) {
		return (i >= '0' && i <= '9');
}

int lex (LEXER *l) {
    register char *p;
    register int c;
    int next;

    p = l->token;
    *p = 0;
    l->tok = 0;

label_top:
    c = l->text [ l->pos ];

    //#############   REMOVE SPACES  ############
    //
    if (c <= 32) {
        if (c == 0) {
            return (l->tok = 0);
        }
        if (c == '\n') {
            l->line++;
				}
        l->pos++; // <<<<<<<<<<  imcrement text position  >>>>>>>>>>
        goto label_top;
    }

    //################  STRING  #################
    //
    if (c == '"') {
        l->pos++; // '"'
        while ((c=l->text[l->pos]) && c != '"' && c != '\r' && c != '\n') {
            l->pos++;
            *p++ = c;
        }
        *p = 0;
        if (c=='"') l->pos++; else Erro("%s: Line: %d - String erro", l->name, l->line);
        return (l->tok = TOK_STRING);
    }

    //##########  WORD,IDENTIFIER ...  ##########
    //
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_') {
        for (;;) {
            c = l->text [ l->pos ];
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_') {
                l->pos++;
                *p++ = c;
            } else break;
        }
        *p = 0;

        if (!strcmp(l->token, "int"))       return (l->tok = TOK_INT);
        if (!strcmp(l->token, "var"))       return (l->tok = TOK_VAR);
        if (!strcmp(l->token, "asm"))       return (l->tok = TOK_ASM);
        if (!strcmp(l->token, "if"))        return (l->tok = TOK_IF);
        if (!strcmp(l->token, "for"))       return (l->tok = TOK_FOR);
        if (!strcmp(l->token, "break"))     return (l->tok = TOK_BREAK);
        if (!strcmp(l->token, "function"))  return (l->tok = TOK_FUNCTION);
        if (!strcmp(l->token, "include"))   return (l->tok = TOK_INCLUDE);
        if (!strcmp(l->token, "module"))    return (l->tok = TOK_MODULE);
        if (!strcmp(l->token, "import"))    return (l->tok = TOK_IMPORT);
        if (!strcmp(l->token, "define"))    return (l->tok = TOK_DEFINE);
        if (!strcmp(l->token, "ifdef"))     { l->ifdef_block++; return (l->tok = TOK_IFDEF); }
        if (!strcmp(l->token, "endif"))     { l->ifdef_block--; return (l->tok = TOK_ENDIF); }

        return (l->tok = TOK_ID);
    }

    //#################  NUMBER  ################
    //
    if (c >= '0' && c <= '9') {
        for (;;) {
            c = l->text [ l->pos ];
            if ((c >= '0' && c <= '9') || c == '.') {
                l->pos++;
                *p++ = c;
            } else break;
        }
        *p = 0;
        return (l->tok = TOK_NUMBER);
    }

    //##########  REMOVE COMMENTS  ##########
    //
    if (c == '/') {
        if (l->text[l->pos+1] == '*') { // comment block
            l->pos += 2;
            do {
                while ((c=l->text[l->pos]) && c != '*') {
                    if (c == '\n') l->line++; //<<<<<<<<<<  line++  >>>>>>>>>>
                    l->pos++;
                }
                if (c) {
                    l->pos++;
                    c = l->text[l->pos];
                }
            } while (c && c != '/');
            if (c == '/') l->pos++;
            else          Erro ("BLOCK COMMENT ERRO: '/'");
            goto label_top;

        } else if (l->text[l->pos+1] == '/') { // comment line
            l->pos += 2;
            while ((c=l->text[l->pos]) && (c != '\n') && (c != '\r'))
                l->pos++;
            goto label_top;
        }
    }//: if (c == '/')

    //---------------------------------------------------------------
    //################## ! C double character ...  ##################
    //---------------------------------------------------------------

    next = l->text[ l->pos+1 ];
    *p++ = c;
    *p = 0;

    if (c=='+') {
        if (next == '+') { // ++
            *p++ = '+'; *p = 0;
            l->pos += 2;
            return (l->tok = TOK_PLUS_PLUS);
        }
        if (next == '=') { // +=
            *p++ = '='; *p = 0;
            l->pos += 2;
            return (l->tok = TOK_PLUS_EQUAL);
        }
    }
    if (c=='-') {
        if (next=='-') { // --
            *p++ = '-'; *p=0;
            l->pos += 2;
            return (l->tok = TOK_MINUS_MINUS);
        }
        if (isdigit(next)) { // number: -100
            l->pos++;
            for (;;) {
                c = l->text[l->pos];
                if ((c >= '0' && c <= '9') || c == '.') {
                    l->pos++;
                    *p++ = c;
                } else break;
            }
            *p = 0;
            return (l->tok = TOK_NUMBER);
        }
    }
    if (c=='=' && next == '=') { // ==
        *p++ = '='; *p = 0;
        l->pos += 2;
        return (l->tok = TOK_EQUAL_EQUAL);
    }
    if (c=='&' && next == '&') { // &&
        *p++ = '&'; *p = 0;
        l->pos += 2;
        return (l->tok = TOK_AND_AND);
    }
    if (c=='|' && next == '|') { // ||
        *p++ = '|'; *p = 0;
        l->pos += 2;
        return (l->tok = TOK_OR_OR);
    }
    if (c=='!' && next == '=') { // !=
        *p++ = '='; *p = 0;
        l->pos += 2;
        return (l->tok = TOK_NOT_EQUAL);
    }

    l->pos++;
    return (l->tok = c);
}

int lex_set (LEXER *l, char *text, char *name) {
    if (l && text && name) {
        l->text = text;
        strcpy (l->name, name);
        l->tok = 0;
        l->pos = 0;
        l->line = 1;
        l->level = 0;
        l->ifdef_block = 0;
        return 1;
    }
    return 0;
}

void lex_save (LEXER *l) {
    sprintk (save_token, "%s", l->token);
    save_pos  = l->pos;
    save_tok  = l->tok;
    save_line = l->line;
}

void lex_restore (LEXER *l) {
    sprintk (l->token, "%s", save_token);
    l->pos  = save_pos;
    l->tok  = save_tok;
    l->line = save_line;
}


