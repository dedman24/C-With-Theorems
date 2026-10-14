#ifndef SATISFIABLE_COMMON_TOK_H_INCLUDED
#define SATISFIABLE_COMMON_TOK_H_INCLUDED

// vfasttables is such a nice utility, it can create tables like these in basically no time.
// only issue I see with it is that to add even a few tokens it takes far longer than what one would like.
// it's a tradeoff between dev speed & code speed that I'd always take. the end result is so fast.

#include "fposT.h"        // satisfiable_fposT.

// stdlib includes.
#include "stdint.h"       // integer types.
#include "stddef.h"       // size_t, NULL.

typedef uintptr_t satisfiable__representation_tokT;
#define SATISFIABLE_TOK__MAX UINTPTR_MAX

// enum needs to be large enough to hold a ptr for code to properly handle 'include' statements.
typedef enum: satisfiable__representation_tokT{
    SATISFIABLE_TOK__ANDL = 64,
    SATISFIABLE_TOK__COLON = 65,
    SATISFIABLE_TOK__ROR = 24,
    SATISFIABLE_TOK__CLOSED_ANGLE_BRACKET = 53,
    SATISFIABLE_TOK__ROL = 48,
    SATISFIABLE_TOK__UNARY_DECREMENT = 70,
    SATISFIABLE_TOK__QUOTE = 37,
    SATISFIABLE_TOK__CLOSED_ROUND_BRACKET = 17,
    SATISFIABLE_TOK__SLASH = 43,
    SATISFIABLE_TOK__AMPERSAND = 67,
    SATISFIABLE_TOK__SHL = 26,
    SATISFIABLE_TOK__IF = 54,
    SATISFIABLE_TOK__IN = 52,
    SATISFIABLE_TOK__EXCLAMATION = 3,
    SATISFIABLE_TOK__PERIOD = 19,
    SATISFIABLE_TOK__OPENED_ROUND_BRACKET = 5,
    SATISFIABLE_TOK__EQ = 28,
    SATISFIABLE_TOK__QUESTION = 7,
    SATISFIABLE_TOK__MATCH = 13,
    SATISFIABLE_TOK__ARROW = 8,
    SATISFIABLE_TOK__BAR = 9,
    SATISFIABLE_TOK__OPENED_ANGLE_BRACKET = 29,
    SATISFIABLE_TOK__CLOSED_CURLY_BRACKET = 31,
    SATISFIABLE_TOK__DOUBLE_QUOTE = 55,
    SATISFIABLE_TOK__MINUS = 73,
    SATISFIABLE_TOK__THEOREM = 34,
    SATISFIABLE_TOK__CLOSED_SQUARE_BRACKET = 45,
    SATISFIABLE_TOK__ASSIGNMENT = 47,
    SATISFIABLE_TOK__PERCENT = 11,
    SATISFIABLE_TOK__NOT = 36,
    SATISFIABLE_TOK__OPENED_SINGLE_LINE_COMMENT = 40,
    SATISFIABLE_TOK__ORL = 6,
    SATISFIABLE_TOK__PLUS = 15,
    SATISFIABLE_TOK__SORRY = 49,
    SATISFIABLE_TOK__OPENED_SQUARE_BRACKET = 1,
    SATISFIABLE_TOK__NEQ = 14,
    SATISFIABLE_TOK__XORL = 2,
    SATISFIABLE_TOK__OPENED_CURLY_BRACKET = 61,
    SATISFIABLE_TOK__CARAT = 21,
    SATISFIABLE_TOK__STAR = 25,
    SATISFIABLE_TOK__ELSE = 0,
    SATISFIABLE_TOK__TILDE = 35,
    SATISFIABLE_TOK__SEMICOLON = 51,
    SATISFIABLE_TOK__FOR = 38,
    SATISFIABLE_TOK__INCLUDE = 42,
    SATISFIABLE_TOK__OPENED_MULTI_LINE_COMMENT = 71,
    SATISFIABLE_TOK__COMMA = 23,
    SATISFIABLE_TOK__GTEQ = 27,
    SATISFIABLE_TOK__BACKSLASH = 33,
    SATISFIABLE_TOK__UNARY_INCREMENT = 12,
    SATISFIABLE_TOK__RETURN = 4,
    SATISFIABLE_TOK__EXISTS = 63,
    SATISFIABLE_TOK__SHR = 50,
    SATISFIABLE_TOK__SELF = 30,
    SATISFIABLE_TOK__LTEQ = 39,
// other types of tokens.
    SATISFIABLE_TOK__END = 80,
    SATISFIABLE_TOK__EOF,
    SATISFIABLE_TOK__NAME,
    SATISFIABLE_TOK__VALUE,
    SATISFIABLE_TOK__NEWLINE,
    SATISFIABLE_TOK__WHITESPACE,
// error-signalling tokens.
// this way of handling errors is convenient, but I'm unsure if it's a good idea.
    SATISFIABLE_TOK__SPECIAL_ERROR,                                 // unknown string, no result when tokenising.
    SATISFIABLE_TOK__SPECIAL_IMPOSSIBILITY,                         // impossible result.
    SATISFIABLE_TOK__NAME_BIT = UINTPTR_MAX & ~INTPTR_MAX           // in tokenised code, names have this bit set always.
} satisfiable__underlying_tokT;

// actual token type.
typedef struct{
  satisfiable__underlying_tokT token;
  satisfiable_fposT line;
  satisfiable_fposT offset;
} satisfiable_tokT;

// initialises satisfiable_tokT type.
satisfiable_tokT satisfiable_tok_init(
    const satisfiable__underlying_tokT tok,
    const satisfiable_fposT line, 
    const satisfiable_fposT offset
);

#define satisfiable_tok_init__macro(tok, l, o) ((satisfiable_tokT){ .token = (tok), .line = (l), .offset = o })

// tries to obtain one token & move string ahead.
satisfiable_tokT satisfiable_tok(
    const char* const restrict str,
    size_t* const restrict len,
    const satisfiable_fposT line,
    const satisfiable_fposT offset
);

#ifdef SATISFIABLE_IMPLEMENTATION

// stdlib includes.
#include "string.h"       // strncmp.

satisfiable_tokT satisfiable_tok_init(const satisfiable__underlying_tokT tok, const satisfiable_fposT line, const satisfiable_fposT offset){
  return (satisfiable_tokT){ tok, line, offset };
}

// hash function, generated with vfasttables version 1.1.0.
static satisfiable__underlying_tokT satisfiable_tok__hash(const char* str, const size_t len){
    if(len > 127) return 0;
// associated values generated for hash function.
    const uint8_t associated_values[] = {
        210,135,212,50,177,176,115,106,44,190,143,111,105,8,93,28,
        85,162,189,124,247,24,1,123,64,122,65,183,96,161,113,251,
        26,230,0,232,68,194,222,159,55,237,201,108,240,225,110,250,
        80,122,246,247,100,251,147,85,191,162,45,126,255,164,243,232,
        66,9,1,184,152,18,230,87,23,135,205,124,221,144,255,77,
        221,9,208,229,61,244,155,162,97,191,81,189,77,163,151,46,
        62,127,247,65,128,66,62,227,61,36,93,1,173,46,60,6,
        175,3,239,203,123,20,180,7,218,17,186,3,17,244,158,18
    };
    const size_t indices[] = {
        1, 0
    };
    uint8_t hash = associated_values[len];
    for(size_t i = 0; i < sizeof(indices)/sizeof(*indices); i++){
        hash += associated_values[str[indices[i] % len]];
    }
    hash %= 80;
    return hash;
}

const char* const restrict satisfiable_tok__table[80] = {
    [SATISFIABLE_TOK__ANDL] = "&&",
    [SATISFIABLE_TOK__COLON] = ":",
    [SATISFIABLE_TOK__ROR] = ">>>",
    [SATISFIABLE_TOK__CLOSED_ANGLE_BRACKET] = "<",
    [SATISFIABLE_TOK__ROL] = "<<<",
    [SATISFIABLE_TOK__UNARY_DECREMENT] = "--",
    [SATISFIABLE_TOK__QUOTE] = "'",
    [SATISFIABLE_TOK__CLOSED_ROUND_BRACKET] = ")",
    [SATISFIABLE_TOK__SLASH] = "/",
    [SATISFIABLE_TOK__AMPERSAND] = "&",
    [SATISFIABLE_TOK__SHL] = ">>",
    [SATISFIABLE_TOK__IF] = "if",
    [SATISFIABLE_TOK__IN] = "in",
    [SATISFIABLE_TOK__EXCLAMATION] = "!",
    [SATISFIABLE_TOK__PERIOD] = ".",
    [SATISFIABLE_TOK__OPENED_ROUND_BRACKET] = "(",
    [SATISFIABLE_TOK__EQ] = "==",
    [SATISFIABLE_TOK__QUESTION] = "?",
    [SATISFIABLE_TOK__MATCH] = "match",
    [SATISFIABLE_TOK__ARROW] = "->",
    [SATISFIABLE_TOK__BAR] = "|",
    [SATISFIABLE_TOK__OPENED_ANGLE_BRACKET] = ">",
    [SATISFIABLE_TOK__CLOSED_CURLY_BRACKET] = "}",
    [SATISFIABLE_TOK__DOUBLE_QUOTE] = "\"",
    [SATISFIABLE_TOK__MINUS] = "-",
    [SATISFIABLE_TOK__THEOREM] = "theorem",
    [SATISFIABLE_TOK__CLOSED_SQUARE_BRACKET] = "]",
    [SATISFIABLE_TOK__ASSIGNMENT] = "=",
    [SATISFIABLE_TOK__PERCENT] = "%",
    [SATISFIABLE_TOK__NOT] = "not",
    [SATISFIABLE_TOK__OPENED_SINGLE_LINE_COMMENT] = "//",
    [SATISFIABLE_TOK__ORL] = "||",
    [SATISFIABLE_TOK__PLUS] = "+",
    [SATISFIABLE_TOK__SORRY] = "sorry",
    [SATISFIABLE_TOK__OPENED_SQUARE_BRACKET] = "[",
    [SATISFIABLE_TOK__NEQ] = "!=",
    [SATISFIABLE_TOK__XORL] = "^^",
    [SATISFIABLE_TOK__OPENED_CURLY_BRACKET] = "{",
    [SATISFIABLE_TOK__CARAT] = "^",
    [SATISFIABLE_TOK__STAR] = "*",
    [SATISFIABLE_TOK__ELSE] = "else",
    [SATISFIABLE_TOK__TILDE] = "~",
    [SATISFIABLE_TOK__SEMICOLON] = ";",
    [SATISFIABLE_TOK__FOR] = "for",
    [SATISFIABLE_TOK__INCLUDE] = "include",
    [SATISFIABLE_TOK__OPENED_MULTI_LINE_COMMENT] = "/*",
    [SATISFIABLE_TOK__COMMA] = ",",
    [SATISFIABLE_TOK__GTEQ] = ">=",
    [SATISFIABLE_TOK__BACKSLASH] = "\\",
    [SATISFIABLE_TOK__UNARY_INCREMENT] = "++",
    [SATISFIABLE_TOK__RETURN] = "return",
    [SATISFIABLE_TOK__EXISTS] = "exists",
    [SATISFIABLE_TOK__SHR] = "<<",
    [SATISFIABLE_TOK__SELF] = "self",
    [SATISFIABLE_TOK__LTEQ] = "<="
/* many tokens, not 100% perfect hashing but this is as close as it would let me. */
};

static satisfiable__underlying_tokT satisfiable_tok__search(const char* const restrict str, const size_t len){
    const satisfiable__underlying_tokT index = satisfiable_tok__hash(str, len);

    return satisfiable_tok__table[index] && strncmp(satisfiable_tok__table[index], str, len) == 0? 
        index: SATISFIABLE_TOK__SPECIAL_ERROR;
}

static size_t satisfiable_tok__whitespacelen(const char* const restrict str){
    return strspn(str, " \t\v\r\f");
}

static size_t satisfiable_tok__oplen(const char* const restrict str){
    return strspn(str, "+-*/%:;?&|^~!~=><()[]{}\"',.\\");
}

// supports binary. hex. octal & fp encoding.
// all of these share that the starting digit is 0 through 9.
static size_t satisfiable_tok__numlen(const char* const restrict str){
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

static size_t satisfiable_tok__codelen(const char* const restrict str){
    const char sp_first = str[0] & ~0x20;
    if(
        'A' <= sp_first && 'Z' >= sp_first ||
        str[0] == '_' ||
        str[0] == '$'
    )
        return strspn(str, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_$");
    else return 0;
}

static satisfiable__underlying_tokT satisfiable_tok__gettok(const char* const restrict str, const size_t len){
    const satisfiable__underlying_tokT tok = satisfiable_tok__search(str, len);
    if(tok == SATISFIABLE_TOK__SPECIAL_ERROR)
        return SATISFIABLE_TOK__NAME;
    return tok;
}

static satisfiable__underlying_tokT satisfiable_tok__gettok_op(const char* const restrict str, size_t* const restrict len){
    satisfiable__underlying_tokT tok;
    while((tok = satisfiable_tok__search(str, *len)) == SATISFIABLE_TOK__SPECIAL_ERROR && *len != 0)
        *len -= 1;
    return tok;
}

// full parsing function, returns token & length of token.
// one just has to call this & the token is returned.
// str ~ null-terminated string holding code.
// len ~ length of section of text to tokenise; must not contain any whitespaces.
// may return 'SATISFIABLE_TOK__SPECIAL_IMPOSSIBILITY', which has to be handled by the underlying machine.
satisfiable_tokT satisfiable_tok(const char* const restrict str, size_t* const restrict len, const satisfiable_fposT line, const satisfiable_fposT offset){
// the way returning stuff's handled is very ugly.
// checks for newlines & whitespaces.
    if(str[0] == '\0') return satisfiable_tok_init(SATISFIABLE_TOK__EOF, line, offset);
    else if(str[0] == '\n'){ *len = 1; return satisfiable_tok_init(SATISFIABLE_TOK__NEWLINE, line, offset); }

    const size_t whitespacelen = satisfiable_tok__whitespacelen(str);
    if(whitespacelen){ *len = whitespacelen; return satisfiable_tok_init(SATISFIABLE_TOK__WHITESPACE, line, offset); }

    *len = satisfiable_tok__codelen(str);
    if(*len) return satisfiable_tok_init(satisfiable_tok__gettok(str, *len), line, offset);

    *len = satisfiable_tok__numlen(str);
    if(*len) return satisfiable_tok_init(SATISFIABLE_TOK__VALUE, line, offset);

    *len = satisfiable_tok__oplen(str);
    if(*len)
        return satisfiable_tok_init(satisfiable_tok__gettok_op(str, len), line, offset);
// technically nothing should be able to get here.
    *len = 1;                   // we have to do this or else no program gets created.
    return satisfiable_tok_init(SATISFIABLE_TOK__SPECIAL_IMPOSSIBILITY, line, offset);
}

#endif
#endif
