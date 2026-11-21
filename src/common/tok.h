#ifndef CTH_COMMON_TOK_H_INCLUDED
#define CTH_COMMON_TOK_H_INCLUDED

#include "fposT.h"        // fposT.

// stdlib includes.
#include "stddef.h"       // size_t, NULL.
#include "stdint.h"       // integer types.
#include "string.h"       // strncmp.

typedef uintptr_t cth_nameT;

typedef enum{
  CTH_TOK__STAR = 57,
  CTH_TOK__GTEQ = 102,
  CTH_TOK__ASSIGN_SLASH = 113,
  CTH_TOK__CLOSED_ANGLE_BRACKET = 11,
  CTH_TOK__ASSIGN_PERCENT = 48,
  CTH_TOK__CLOSED_ROUND_BRACKET = 13,
  CTH_TOK__LTEQ = 84,
  CTH_TOK__ASSIGN_ROL = 58,
  CTH_TOK__ASSIGN_AMPERSAND = 117,
  CTH_TOK__ROR = 30,
  CTH_TOK__ASSIGN_ROR = 78,
  CTH_TOK__PERCENT = 59,
  CTH_TOK__CLOSED_SQUARE_BRACKET = 15,
  CTH_TOK__SIZEOF = 27,
  CTH_TOK__OPENED_ANGLE_BRACKET = 31,
  CTH_TOK__QUOTE = 61,
  CTH_TOK__ASSIGN_STAR = 39,
  CTH_TOK__CARAT = 71,
  CTH_TOK__ASSIGN_BAR = 36,
  CTH_TOK__COUNTOF = 42,
  CTH_TOK__BACKSLASH = 73,
  CTH_TOK__XORL = 80,
  CTH_TOK__EXISTS = 43,
  CTH_TOK__ASSIGN_PLUS = 9,
  CTH_TOK__PREPROCESSOR = 17,
  CTH_TOK__ROL = 10,
  CTH_TOK__TYPEOF = 18,
  CTH_TOK__CONTINUE = 1,
  CTH_TOK__BREAK = 3,
  CTH_TOK__EQ = 28,
  CTH_TOK__REFL = 32,
  CTH_TOK__OPENED_CURLY_BRACKET = 23,
  CTH_TOK__ORL = 44,
  CTH_TOK__PLUS = 85,
  CTH_TOK__ELSE = 93,
  CTH_TOK__SWITCH = 5,
  CTH_TOK__UNARY_INCREMENT = 94,
  CTH_TOK__OPENED_SINGLE_LINE_COMMENT = 62,
  CTH_TOK__COMMA = 29,
  CTH_TOK__ARROW = 118,
  CTH_TOK__UNARY_DECREMENT = 76,
  CTH_TOK__CLOSED_CURLY_BRACKET = 45,
  CTH_TOK__DOUBLE_QUOTE = 75,
  CTH_TOK__ERROR = 19,
  CTH_TOK__DO = 33,
  CTH_TOK__TYPEIS = 82,
  CTH_TOK__BAR = 51,
  CTH_TOK__DEFAULT = 110,
  CTH_TOK__SLASH = 53,
  CTH_TOK__AMPERSAND = 77,
  CTH_TOK__OPENED_SQUARE_BRACKET = 119,
  CTH_TOK__RETURN = 37,
  CTH_TOK__SHR = 20,
  CTH_TOK__PERIOD = 79,
  CTH_TOK__OPENED_MULTI_LINE_COMMENT = 4,
  CTH_TOK__TILDE = 95,
  CTH_TOK__CASE = 0,
  CTH_TOK__ASSIGN_SHR = 92,
  CTH_TOK__ASSIGNMENT = 35,
  CTH_TOK__ASSIGN_SHL = 90,
  CTH_TOK__FOR = 104,
  CTH_TOK__INCLUDE = 22,
  CTH_TOK__COLON = 89,
  CTH_TOK__NEQ = 34,
  CTH_TOK__DEFER = 101,
  CTH_TOK__WHILE = 55,
  CTH_TOK__ASSIGN_CARAT = 2,
  CTH_TOK__SORRY = 7,
  CTH_TOK__SHL = 40,
  CTH_TOK__EXCLAMATION = 47,
  CTH_TOK__MINUS = 83,
  CTH_TOK__QUESTION = 41,
  CTH_TOK__ANDL = 86,
  CTH_TOK__SEMICOLON = 97,
  CTH_TOK__IF = 87,
  CTH_TOK__OPENED_ROUND_BRACKET = 103,
  CTH_TOK__TYPEDEF = 81,
  CTH_TOK__ASSIGN_MINUS = 60,
  CTH_TOK__UNRESOLVABLE = 96,
  CTH_TOK__NONE = 120,

  CTH_TOK__DECLARATION,

  CTH_TOK__UNARY_START,
  CTH_TOK__UNARY_PLUS,
  CTH_TOK__UNARY_MINUS,
  CTH_TOK__UNARY_STAR,
  CTH_TOK__UNARY_AMPERSAND,
  CTH_TOK__UNARY_SIZEOF,
  CTH_TOK__UNARY_TYPEOF,
  CTH_TOK__UNARY_COUNTOF,
  CTH_TOK__UNARY_END,

  CTH_TOK__NAME,
  CTH_TOK__VALUE,
  CTH_TOK__CHAR,
  CTH_TOK__STRING,
  CTH_TOK__FUNCALL,

  CTH_TOK__SPECIAL_IMPOSSIBILITY,
  CTH_TOK__WHITESPACE,
  CTH_TOK__NEWLINE
} cth_tokT;

cth_tokT cth_tok(const char* const restrict str, size_t* const restrict len);

// NOTE: how do we handle certain C ops having different associativity directions (L->R vs R->L)?
typedef enum{
  CTH_PRIORITY__LOWEST,                                     //
  CTH_PRIORITY__SEMICOLON,                                  // ';'.
  CTH_PRIORITY__DECLARATION,                                // variable declarations.
  CTH_PRIORITY__COMMA,                                      // ','.
  CTH_PRIORITY__TYPEIS,                                     // '::'.
  CTH_PRIORITY__ASSIGNMENT,                                 // '='.
  CTH_PRIORITY__IF,                                         // 'if', ternary if.
  CTH_PRIORITY__LOGICAL_OR,                                 // '||'.
  CTH_PRIORITY__LOGICAL_XOR,                                // '^^'.
  CTH_PRIORITY__LOGICAL_AND,                                // '&&'.
  CTH_PRIORITY__BITWISE_OR,                                 // '|'.
  CTH_PRIORITY__BITWISE_XOR,                                // '^'.
  CTH_PRIORITY__BITWISE_AND,                                // '&'.
  CTH_PRIORITY__EQUALITY,                                   // '==', '!='.
  CTH_PRIORITY__RELATIONS,                                  // '>', '>=', '<', '<='.
  CTH_PRIORITY__SHIFTS,                                     // '<<', '>>', '<<<', '>>>'.
  CTH_PRIORITY__SIMPLE_ARITH,                               // '+', binary '-'.
  CTH_PRIORITY__COMPLEX_ARITH,                              // '*', '/', '%'.
  CTH_PRIORITY__2,                                          // second highest priority level.
  CTH_PRIORITY__1,                                          // highest priority level.
  CTH_PRIORITY__HIGHEST                                     // highest priority level not assigned to operation; this and __LOWEST mght be useful.
} cth_priorityT;

#ifndef CTH_IMPLEMENTATION
extern const cth_priorityT cth_priority[];
extern const uint8_t isunary[];

#else 

const cth_priorityT cth_priority[] ={
  [CTH_TOK__SEMICOLON]            = CTH_PRIORITY__SEMICOLON,
  [CTH_TOK__DECLARATION]          = CTH_PRIORITY__DECLARATION,

  [CTH_TOK__COMMA]                = CTH_PRIORITY__COMMA,
  [CTH_TOK__TYPEIS]               = CTH_PRIORITY__TYPEIS,

  [CTH_TOK__ASSIGNMENT]           = CTH_PRIORITY__ASSIGNMENT,
  [CTH_TOK__ASSIGN_STAR]          = CTH_PRIORITY__ASSIGNMENT,
  [CTH_TOK__ASSIGN_SLASH]         = CTH_PRIORITY__ASSIGNMENT,
  [CTH_TOK__ASSIGN_PERCENT]       = CTH_PRIORITY__ASSIGNMENT,
  [CTH_TOK__ASSIGN_PLUS]          = CTH_PRIORITY__ASSIGNMENT,
  [CTH_TOK__ASSIGN_MINUS]         = CTH_PRIORITY__ASSIGNMENT,
  [CTH_TOK__ASSIGN_AMPERSAND]     = CTH_PRIORITY__ASSIGNMENT,
  [CTH_TOK__ASSIGN_CARAT]         = CTH_PRIORITY__ASSIGNMENT,
  [CTH_TOK__ASSIGN_BAR]           = CTH_PRIORITY__ASSIGNMENT,
  [CTH_TOK__ASSIGN_SHL]           = CTH_PRIORITY__ASSIGNMENT,
  [CTH_TOK__ASSIGN_SHR]           = CTH_PRIORITY__ASSIGNMENT,
  [CTH_TOK__ASSIGN_ROL]           = CTH_PRIORITY__ASSIGNMENT,
  [CTH_TOK__ASSIGN_ROR]           = CTH_PRIORITY__ASSIGNMENT,

  [CTH_TOK__IF]                   = CTH_PRIORITY__IF,
  [CTH_TOK__ELSE]                 = CTH_PRIORITY__IF,
  [CTH_TOK__COLON]                = CTH_PRIORITY__IF,
  [CTH_TOK__QUESTION]             = CTH_PRIORITY__IF,

  [CTH_TOK__ORL]                  = CTH_PRIORITY__LOGICAL_OR,
  [CTH_TOK__XORL]                 = CTH_PRIORITY__LOGICAL_XOR,
  [CTH_TOK__ANDL]                 = CTH_PRIORITY__LOGICAL_AND,

  [CTH_TOK__BAR]                  = CTH_PRIORITY__BITWISE_OR,
  [CTH_TOK__CARAT]                = CTH_PRIORITY__BITWISE_XOR,
  [CTH_TOK__AMPERSAND]            = CTH_PRIORITY__BITWISE_AND,
  
  [CTH_TOK__EQ]                   = CTH_PRIORITY__EQUALITY,
  [CTH_TOK__NEQ]                  = CTH_PRIORITY__EQUALITY,

  [CTH_TOK__OPENED_ANGLE_BRACKET] = CTH_PRIORITY__RELATIONS,
  [CTH_TOK__CLOSED_ANGLE_BRACKET] = CTH_PRIORITY__RELATIONS,
  [CTH_TOK__GTEQ]                 = CTH_PRIORITY__RELATIONS,
  [CTH_TOK__LTEQ]                 = CTH_PRIORITY__RELATIONS,

  [CTH_TOK__SHL]                  = CTH_PRIORITY__SHIFTS,
  [CTH_TOK__SHR]                  = CTH_PRIORITY__SHIFTS,
  [CTH_TOK__ROL]                  = CTH_PRIORITY__SHIFTS,
  [CTH_TOK__ROR]                  = CTH_PRIORITY__SHIFTS,

  [CTH_TOK__PLUS]                 = CTH_PRIORITY__SIMPLE_ARITH,
  [CTH_TOK__MINUS]                = CTH_PRIORITY__SIMPLE_ARITH,

  [CTH_TOK__STAR]                 = CTH_PRIORITY__COMPLEX_ARITH,
  [CTH_TOK__SLASH]                = CTH_PRIORITY__COMPLEX_ARITH,
  [CTH_TOK__PERCENT]              = CTH_PRIORITY__COMPLEX_ARITH,

  [CTH_TOK__UNARY_PLUS]           = CTH_PRIORITY__2,
  [CTH_TOK__UNARY_MINUS]          = CTH_PRIORITY__2,
  [CTH_TOK__UNARY_STAR]           = CTH_PRIORITY__2,
  [CTH_TOK__UNARY_AMPERSAND]      = CTH_PRIORITY__2,

  [CTH_TOK__UNARY_SIZEOF]         = CTH_PRIORITY__2,
  [CTH_TOK__UNARY_TYPEOF]         = CTH_PRIORITY__2,
  [CTH_TOK__UNARY_COUNTOF]        = CTH_PRIORITY__2,

  [CTH_TOK__FUNCALL]              = CTH_PRIORITY__1,
};

// hash function, generated with vfasttables version 1.1.0.
static cth_tokT cth_tok__hash(const char* str, const size_t len){
  if(len > CTH_TOK__NONE - 1) return 0;
// associated values generated for hash function.
  const uint8_t associated_values[] = {
    4,173,46,172,220,161,126,189,24,152,216,177,26,234,14,153,
    231,149,75,148,147,255,227,11,57,208,97,246,191,123,114,161,
    18,125,139,238,195,3,208,72,93,48,130,84,184,15,209,68,
    113,241,252,77,67,73,213,30,117,89,18,90,175,119,57,250,
    63,92,50,210,220,188,31,118,60,255,106,7,188,255,217,225,
    161,81,83,219,201,200,70,199,26,92,192,33,206,49,77,208,
    9,140,49,152,30,238,102,133,200,59,46,31,11,190,81,77,
    81,43,86,55,166,121,217,135,95,194,6,53,127,252,89,144
  };
  const size_t indices[] = {
    5, 0
  };
  uint8_t hash = associated_values[len];
  for(size_t i = 0; i < sizeof(indices)/sizeof(*indices); i++)
    hash += associated_values[str[indices[i] % len]];

  hash %= CTH_TOK__NONE;
  return hash;
}

static cth_tokT cth_tok__search(const char* const restrict str, const size_t len){
  const cth_tokT index = cth_tok__hash(str, len);
  const char* table[CTH_TOK__NONE] = {
    [CTH_TOK__STAR] = "*",
    [CTH_TOK__GTEQ] = ">=",
    [CTH_TOK__ASSIGN_SLASH] = "/=",
    [CTH_TOK__CLOSED_ANGLE_BRACKET] = "<",
    [CTH_TOK__ASSIGN_PERCENT] = "%=",
    [CTH_TOK__CLOSED_ROUND_BRACKET] = ")",
    [CTH_TOK__LTEQ] = "<=",
    [CTH_TOK__ASSIGN_ROL] = "<<<=",
    [CTH_TOK__ASSIGN_AMPERSAND] = "&=",
    [CTH_TOK__ROR] = ">>>",
    [CTH_TOK__ASSIGN_ROR] = ">>>=",
    [CTH_TOK__PERCENT] = "%",
    [CTH_TOK__CLOSED_SQUARE_BRACKET] = "]",
    [CTH_TOK__SIZEOF] = "sizeof",
    [CTH_TOK__OPENED_ANGLE_BRACKET] = ">",
    [CTH_TOK__QUOTE] = "'",
    [CTH_TOK__ASSIGN_STAR] = "*=",
    [CTH_TOK__CARAT] = "^",
    [CTH_TOK__ASSIGN_BAR] = "|=",
    [CTH_TOK__COUNTOF] = "countof",
    [CTH_TOK__BACKSLASH] = "\\",
    [CTH_TOK__XORL] = "^^",
    [CTH_TOK__EXISTS] = "exists",
    [CTH_TOK__ASSIGN_PLUS] = "+=",
    [CTH_TOK__PREPROCESSOR] = "#",
    [CTH_TOK__ROL] = "<<<",
    [CTH_TOK__TYPEOF] = "typeof",
    [CTH_TOK__CONTINUE] = "continue",
    [CTH_TOK__BREAK] = "break",
    [CTH_TOK__EQ] = "==",
    [CTH_TOK__REFL] = "refl",
    [CTH_TOK__OPENED_CURLY_BRACKET] = "{",
    [CTH_TOK__ORL] = "||",
    [CTH_TOK__PLUS] = "+",
    [CTH_TOK__ELSE] = "else",
    [CTH_TOK__SWITCH] = "switch",
    [CTH_TOK__UNARY_INCREMENT] = "++",
    [CTH_TOK__OPENED_SINGLE_LINE_COMMENT] = "//",
    [CTH_TOK__COMMA] = ",",
    [CTH_TOK__ARROW] = "->",
    [CTH_TOK__UNARY_DECREMENT] = "--",
    [CTH_TOK__CLOSED_CURLY_BRACKET] = "}",
    [CTH_TOK__DOUBLE_QUOTE] = "\"",
    [CTH_TOK__ERROR] = "@error",
    [CTH_TOK__DO] = "do",
    [CTH_TOK__TYPEIS] = "::",
    [CTH_TOK__BAR] = "|",
    [CTH_TOK__DEFAULT] = "default",
    [CTH_TOK__SLASH] = "/",
    [CTH_TOK__AMPERSAND] = "&",
    [CTH_TOK__OPENED_SQUARE_BRACKET] = "[",
    [CTH_TOK__RETURN] = "return",
    [CTH_TOK__SHR] = "<<",
    [CTH_TOK__PERIOD] = ".",
    [CTH_TOK__OPENED_MULTI_LINE_COMMENT] = "/*",
    [CTH_TOK__TILDE] = "~",
    [CTH_TOK__CASE] = "case",
    [CTH_TOK__ASSIGN_SHR] = ">>=",
    [CTH_TOK__ASSIGNMENT] = "=",
    [CTH_TOK__ASSIGN_SHL] = "<<=",
    [CTH_TOK__FOR] = "for",
    [CTH_TOK__INCLUDE] = "include",
    [CTH_TOK__COLON] = ":",
    [CTH_TOK__NEQ] = "!=",
    [CTH_TOK__DEFER] = "defer",
    [CTH_TOK__WHILE] = "while",
    [CTH_TOK__ASSIGN_CARAT] = "^=",
    [CTH_TOK__SORRY] = "@sorry",
    [CTH_TOK__SHL] = ">>",
    [CTH_TOK__EXCLAMATION] = "!",
    [CTH_TOK__MINUS] = "-",
    [CTH_TOK__QUESTION] = "?",
    [CTH_TOK__ANDL] = "&&",
    [CTH_TOK__SEMICOLON] = ";",
    [CTH_TOK__IF] = "if",
    [CTH_TOK__OPENED_ROUND_BRACKET] = "(",
    [CTH_TOK__TYPEDEF] = "typedef",
    [CTH_TOK__ASSIGN_MINUS] = "-=",
    [CTH_TOK__UNRESOLVABLE] = "@unresolvable"
  };
  return table[index] && strncmp(table[index], str, len) == 0? index: CTH_TOK__NONE;
}

static size_t cth_tok__oplen(const char* const restrict str){
  return strspn(str, "+-*/%:;?&|^~!~=><()[]{},.\\#");
}

// find position of terminator.
// if preceeded by '\', find next.
static size_t cth_tok__strlen(const char* const restrict base, const char terminator){
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
static size_t cth_tok__numlen(const char* const restrict str){
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

static size_t cth_tok__codelen(const char* const restrict str){
  const char sp_first = str[0] & ~0x20;
  if(
    'A' <= sp_first && 'Z' >= sp_first ||
    str[0] == '_' ||
    str[0] == '$'
  )
    return strspn(str, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_$@");
  else return 0;
}

static cth_tokT cth_tok__gettok(const char* const restrict str, const size_t len){
  const cth_tokT tok = cth_tok__search(str, len);
  if(tok == CTH_TOK__NONE)
    return CTH_TOK__NAME;
  return tok;
}

static cth_tokT cth_tok__gettok_op(const char* const restrict str, size_t* const restrict len){
  cth_tokT tok;
  while((tok = cth_tok__search(str, *len)) == CTH_TOK__NONE && *len != 0)
    *len -= 1;
  return tok;
}

// full parsing function, returns token & length of token.
// one just has to call this & the token is returned.
// str ~ null-terminated string holding code.
// len ~ length of section of text to tokenise; must not contain any whitespaces.
// may return 'cth_TOK__SPECIAL_IMPOSSIBILITY', which has to be handled by the underlying machine.
cth_tokT cth_tok(const char* const restrict str, size_t* const restrict len){
  if(str[0] == '\n'){ *len = 1; return CTH_TOK__NEWLINE; }

  const bool iswhitespace = strchr(" \t\v\r\f", str[0]);
  if(iswhitespace){ *len = strspn(str, " \t\v\r\f"); return CTH_TOK__WHITESPACE; }
  if(str[0] == '\''){ *len = cth_tok__strlen(str + 1, '\'') + 1; return CTH_TOK__CHAR; }
  if(str[0] == '\"'){ *len = cth_tok__strlen(str + 1, '\"') + 1; return CTH_TOK__STRING; }

  *len = cth_tok__numlen(str);
  if(*len) return CTH_TOK__VALUE;

  *len = cth_tok__codelen(str);
  if(*len) return cth_tok__gettok(str, *len);

  *len = cth_tok__oplen(str);
  if(*len)
    return cth_tok__gettok_op(str, len);
// technically nothing should be able to get here.
  *len = 1;                   // we have to do this or else no program gets compiled.
  return CTH_TOK__SPECIAL_IMPOSSIBILITY;
}

#endif
#endif
