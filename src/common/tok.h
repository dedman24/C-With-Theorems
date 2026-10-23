#ifndef CTH_COMMON_TOK_H_INCLUDED
#define CTH_COMMON_TOK_H_INCLUDED

#include "fposT.h"                  // fposT.

// stdlib includes.
#include "stdint.h"                 // uintptr_t.
#include "string.h"                 // strspn, strcspn.
#include "stdbool.h"                // bool, true, false.

typedef uintptr_t cth__underlying_tokT;
#define CTH_TOK__MAX UINTPTR_MAX

typedef cth__underlying_tokT cth_nameT;

typedef enum{
  CTH_TOK__NAME,
  CTH_TOK__CHAR,
  CTH_TOK__STRING,
  CTH_TOK__VALUE,
  CTH_TOK__OPENED_BRACKET,
  CTH_TOK__CLOSED_BRACKET,
  CTH_TOK__WHITESPACE,
  CTH_TOK__NEWLINE,
  CTH_TOK__COMMENT,
  CTH_TOK__EOF
} cth_tok_typeT;

cth_tok_typeT cth_tok(const char* const restrict str, size_t* const restrict len);

typedef struct{
  cth_nameT name;
  cth_fposT line;
  cth_fposT offset;
} cth_tokT;

#define cth_tok_init(n, l, off) ((cth_tokT){ .name = n, .line = l, .offset = off })

#ifdef CTH_IMPLEMENTATION

// find position of terminator.
// if preceeded by '\', find next.
static size_t cth_tok__str(const char* const restrict base, const char terminator){
    const char* off = base + 1;                         // to skip initial character.
    while(1){
        const char* const term = strchr(off, terminator);
        if(!term) return strlen(base);
        if(off[-1] != '\\') return off - base;
        else off = term + 1;
    }
}

// supports binary. hex. octal & fp encoding.
// all of these share that the starting digit is 0 through 9.
static size_t cth_tok__num(const char* const restrict str){
    if(str[0] == '"') return cth_tok__str(str, '"');
    if(str[0] == '\'') return cth_tok__str(str, '\'');
// first char is digit?
//  y -> strspn.
//  n -> first char is - or .?
//      y -> second char is . or digit?
//          y -> strspn.
//          n -> return 0.
//      n -> return 0.
// this can still pass values like '..0', which is wrong.
    #define CHARACTER_IS_DIGIT(c) ((unsigned char)(c) - '0' <= 9)
    if(CHARACTER_IS_DIGIT(str[0])) 
        return strspn(str, "0123456789ABCDEFIOXabcdefiox.");
    if(str[0] != '-' && str[0] != '.') return 0;
    if(!CHARACTER_IS_DIGIT(str[1]) && str[1] != '.') return 0;
    return 1 + strspn(str + 1, "0123456789ABCDEFIOXabcdefiox.");
    #undef CHARACTER_IS_DIGIT
}

const size_t cth_tok__whitespace(const char* const restrict str){
  return strspn(str, " \t\v\r\f");
}

const size_t cth_tok__notwhitespace(const char* const restrict str){
  return strcspn(str, " \t\v\r\f");
}

cth_tok_typeT cth_tok(const char* const restrict str, size_t* const restrict len){
  if(str[0] == '\0'){ 
    *len = 1;
    return CTH_TOK__EOF;
  }
  const bool iswhitespace = strchr(" \t\v\r\f", str[0]) != NULL;
  if(iswhitespace){
    *len = cth_tok__whitespace(str);
    return CTH_TOK__WHITESPACE;
  }

  if(str[0] == '#') return CTH_TOK__COMMENT;
  if(str[0] == '\n'){ *len = 1; return CTH_TOK__NEWLINE; }
  if(str[0] == '('){ *len = 1; return CTH_TOK__OPENED_BRACKET; }
  if(str[0] == ')'){ *len = 1; return CTH_TOK__OPENED_BRACKET; }
  if(str[0] == '\''){ *len = cth_tok__str(str + 1, '\'') + 1; return CTH_TOK__CHAR; }
  if(str[0] == '\"'){ *len = cth_tok__str(str + 1, '\"') + 1; return CTH_TOK__STRING; }
  const bool isdigit = strchr("0123456789", str[0]) != NULL;
  if(isdigit){
    *len = cth_tok__num(str);
    return CTH_TOK__VALUE;
  }

  *len = cth_tok__notwhitespace(str);
  return CTH_TOK__NAME;
}

#endif
#endif