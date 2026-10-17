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
    SATISFIABLE_TOK__THEOREM = 12,
    SATISFIABLE_TOK__OPENED_MULTI_LINE_COMMENT = 25,
    SATISFIABLE_TOK__IN = 26,
    SATISFIABLE_TOK__RETURN = 0,
    SATISFIABLE_TOK__ROL = 67,
    SATISFIABLE_TOK__TYPEOF = 1,
    SATISFIABLE_TOK__QUESTION = 27,
    SATISFIABLE_TOK__QUOTE = 29,
    SATISFIABLE_TOK__COLON = 31,
    SATISFIABLE_TOK__COUNTOF = 2,
    SATISFIABLE_TOK__ROR = 73,
    SATISFIABLE_TOK__SHR = 52,
    SATISFIABLE_TOK__UNARY_DECREMENT = 30,
    SATISFIABLE_TOK__SLASH = 33,
    SATISFIABLE_TOK__CLOSED_ANGLE_BRACKET = 15,
    SATISFIABLE_TOK__AMPERSAND = 35,
    SATISFIABLE_TOK__CARAT = 37,
    SATISFIABLE_TOK__MINUS = 9,
    SATISFIABLE_TOK__OPENED_SINGLE_LINE_COMMENT = 70,
    SATISFIABLE_TOK__IF = 4,
    SATISFIABLE_TOK__ORL = 18,
    SATISFIABLE_TOK__ASSIGNMENT = 41,
    SATISFIABLE_TOK__PLUS = 43,
    SATISFIABLE_TOK__CLOSED_SQUARE_BRACKET = 45,
    SATISFIABLE_TOK__CLOSED_ROUND_BRACKET = 47,
    SATISFIABLE_TOK__SEMICOLON = 49,
    SATISFIABLE_TOK__OPENED_ROUND_BRACKET = 51,
    SATISFIABLE_TOK__UNARY_INCREMENT = 64,
    SATISFIABLE_TOK__COMMA = 53,
    SATISFIABLE_TOK__STAR = 55,
    SATISFIABLE_TOK__SORRY = 6,
    SATISFIABLE_TOK__OPENED_SQUARE_BRACKET = 57,
    SATISFIABLE_TOK__NEQ = 3,
    SATISFIABLE_TOK__OPENED_CURLY_BRACKET = 59,
    SATISFIABLE_TOK__ANDL = 56,
    SATISFIABLE_TOK__XORL = 74,
    SATISFIABLE_TOK__PERCENT = 61,
    SATISFIABLE_TOK__LTEQ = 17,
    SATISFIABLE_TOK__ELSE = 5,
    SATISFIABLE_TOK__MATCH = 7,
    SATISFIABLE_TOK__GTEQ = 20,
    SATISFIABLE_TOK__DOUBLE_QUOTE = 75,
    SATISFIABLE_TOK__SHL = 58,
    SATISFIABLE_TOK__OPENED_ANGLE_BRACKET = 21,
    SATISFIABLE_TOK__CLOSED_CURLY_BRACKET = 79,
    SATISFIABLE_TOK__BACKSLASH = 39,
    SATISFIABLE_TOK__TILDE = 11,
    SATISFIABLE_TOK__ARROW = 44,
    SATISFIABLE_TOK__PERIOD = 13,
    SATISFIABLE_TOK__BAR = 77,
    SATISFIABLE_TOK__EXCLAMATION = 19,
    SATISFIABLE_TOK__INCLUDE = 50,
    SATISFIABLE_TOK__EQ = 62,
// other types of tokens.
    SATISFIABLE_TOK__END = 80,
// operators that differ not in form but semantics.
    SATISFIABLE_TOK__NEG,                                           // unary -.
    SATISFIABLE_TOK__POS,                                           // unary +.
    SATISFIABLE_TOK__FIELDOF,                                       // binary ~.

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

// properties.
// for x, y : satisfiable__underlying_tokT
//  let a, b : satisfiable_tok__priorityT = satisfiable_tok__priority[x], satisfiable_tok__priority[y];
//  if a < b then x is of lower priority wrt y; if a > b y is of lower priority wrt x.
// Satisfiable is written so that everything is an operation with a given priority, and a simple operator priority parser can work through it.
// this means that parsing is quite fast :DDD, while with C a full AST would've required building.
typedef enum{
    SATISFIABLE_PRIORITY__LOWEST,                                   //
    SATISFIABLE_PRIORITY__EOL,                                      // ';', which is used to mark the end of lines of code.
    SATISFIABLE_PRIORITY__MATCH,                                    // match.
    SATISFIABLE_PRIORITY__LOGIC,                                    // '&&', '||' & so on.
    SATISFIABLE_PRIORITY__EQUALITY,                                 // '==', '!=', '>=', '<='.
    SATISFIABLE_PRIORITY__BITWISE,                                  // '&', '|', '^'.
    SATISFIABLE_PRIORITY__MOD,                                      // '%'.
    SATISFIABLE_PRIORITY__ARITHMETIC,                               // '+', '-'.
    SATISFIABLE_PRIORITY__COMPLEX_ARITHMETIC,                       // '*', '/'.
    SATISFIABLE_PRIORITY__TYPE,                                     // ':'.
    SATISFIABLE_PRIORITY__UNARY,                                    // all unary ops.
    SATISFIABLE_PRIORITY__UNARY_COMPLEX,                            // countof, typeof.
    SATISFIABLE_PRIORITY__ELEMENTOF,                                // '.', binary '~'.
    SATISFIABLE_PRIORITY__HIGHEST
} satisfiable_tok__priorityT;

#ifndef SATISFIABLE_IMPLEMENTATION
extern const satisfiable_tok__priorityT satisfiable_tok__priority[];
extern const char* const restrict satisfiable_tok__table[80];
#endif

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
        74,101,42,233,31,16,229,226,97,24,204,53,253,48,102,156,
        86,251,27,32,243,120,174,52,65,226,31,80,81,138,69,211,
        198,39,243,126,228,148,7,4,55,53,17,179,104,122,84,222,
        129,250,139,103,191,250,125,107,115,214,93,54,213,178,216,219,
        138,214,187,65,230,190,55,12,123,115,4,188,254,248,237,146,
        222,158,111,170,22,146,18,158,194,101,159,186,177,100,224,128,
        23,47,173,143,39,220,230,208,9,68,150,146,90,24,172,49,
        253,207,47,21,113,75,131,112,42,75,10,147,116,29,75,236
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
    [SATISFIABLE_TOK__THEOREM] = "theorem",
    [SATISFIABLE_TOK__OPENED_MULTI_LINE_COMMENT] = "/*",
    [SATISFIABLE_TOK__IN] = "in",
    [SATISFIABLE_TOK__RETURN] = "return",
    [SATISFIABLE_TOK__ROL] = "<<<",
    [SATISFIABLE_TOK__TYPEOF] = "typeof",
    [SATISFIABLE_TOK__QUESTION] = "?",
    [SATISFIABLE_TOK__QUOTE] = "'",
    [SATISFIABLE_TOK__COLON] = ":",
    [SATISFIABLE_TOK__COUNTOF] = "countof",
    [SATISFIABLE_TOK__ROR] = ">>>",
    [SATISFIABLE_TOK__SHR] = "<<",
    [SATISFIABLE_TOK__UNARY_DECREMENT] = "--",
    [SATISFIABLE_TOK__SLASH] = "/",
    [SATISFIABLE_TOK__CLOSED_ANGLE_BRACKET] = "<",
    [SATISFIABLE_TOK__AMPERSAND] = "&",
    [SATISFIABLE_TOK__CARAT] = "^",
    [SATISFIABLE_TOK__MINUS] = "-",
    [SATISFIABLE_TOK__OPENED_SINGLE_LINE_COMMENT] = "//",
    [SATISFIABLE_TOK__IF] = "if",
    [SATISFIABLE_TOK__ORL] = "||",
    [SATISFIABLE_TOK__ASSIGNMENT] = "=",
    [SATISFIABLE_TOK__PLUS] = "+",
    [SATISFIABLE_TOK__CLOSED_SQUARE_BRACKET] = "]",
    [SATISFIABLE_TOK__CLOSED_ROUND_BRACKET] = ")",
    [SATISFIABLE_TOK__SEMICOLON] = ";",
    [SATISFIABLE_TOK__OPENED_ROUND_BRACKET] = "(",
    [SATISFIABLE_TOK__UNARY_INCREMENT] = "++",
    [SATISFIABLE_TOK__COMMA] = ",",
    [SATISFIABLE_TOK__STAR] = "*",
    [SATISFIABLE_TOK__SORRY] = "sorry",
    [SATISFIABLE_TOK__OPENED_SQUARE_BRACKET] = "[",
    [SATISFIABLE_TOK__NEQ] = "!=",
    [SATISFIABLE_TOK__OPENED_CURLY_BRACKET] = "{",
    [SATISFIABLE_TOK__ANDL] = "&&",
    [SATISFIABLE_TOK__XORL] = "^^",
    [SATISFIABLE_TOK__PERCENT] = "%",
    [SATISFIABLE_TOK__LTEQ] = "<=",
    [SATISFIABLE_TOK__ELSE] = "else",
    [SATISFIABLE_TOK__MATCH] = "match",
    [SATISFIABLE_TOK__GTEQ] = ">=",
    [SATISFIABLE_TOK__DOUBLE_QUOTE] = "\"",
    [SATISFIABLE_TOK__SHL] = ">>",
    [SATISFIABLE_TOK__OPENED_ANGLE_BRACKET] = ">",
    [SATISFIABLE_TOK__CLOSED_CURLY_BRACKET] = "}",
    [SATISFIABLE_TOK__BACKSLASH] = "\\",
    [SATISFIABLE_TOK__TILDE] = "~",
    [SATISFIABLE_TOK__ARROW] = "->",
    [SATISFIABLE_TOK__PERIOD] = ".",
    [SATISFIABLE_TOK__BAR] = "|",
    [SATISFIABLE_TOK__EXCLAMATION] = "!",
    [SATISFIABLE_TOK__INCLUDE] = "include",
    [SATISFIABLE_TOK__EQ] = "=="
/* many tokens, not 100% perfect hashing but this is as close as it would let me. */
};

const satisfiable_tok__priorityT satisfiable_tok__priority[] = {
    [SATISFIABLE_TOK__SEMICOLON]            = SATISFIABLE_PRIORITY__EOL,
    [SATISFIABLE_TOK__MATCH]                = SATISFIABLE_PRIORITY__MATCH,

    [SATISFIABLE_TOK__ANDL]                 = SATISFIABLE_PRIORITY__LOGIC,
    [SATISFIABLE_TOK__ORL]                  = SATISFIABLE_PRIORITY__LOGIC,
    [SATISFIABLE_TOK__XORL]                 = SATISFIABLE_PRIORITY__LOGIC,

    [SATISFIABLE_TOK__EQ]                   = SATISFIABLE_PRIORITY__EQUALITY,
    [SATISFIABLE_TOK__NEQ]                  = SATISFIABLE_PRIORITY__EQUALITY,
    [SATISFIABLE_TOK__GTEQ]                 = SATISFIABLE_PRIORITY__EQUALITY,
    [SATISFIABLE_TOK__LTEQ]                 = SATISFIABLE_PRIORITY__EQUALITY,
    [SATISFIABLE_TOK__OPENED_ANGLE_BRACKET] = SATISFIABLE_PRIORITY__EQUALITY,
    [SATISFIABLE_TOK__CLOSED_ANGLE_BRACKET] = SATISFIABLE_PRIORITY__EQUALITY,

    [SATISFIABLE_TOK__AMPERSAND]            = SATISFIABLE_PRIORITY__BITWISE,
    [SATISFIABLE_TOK__BAR]                  = SATISFIABLE_PRIORITY__BITWISE,
    [SATISFIABLE_TOK__CARAT]                = SATISFIABLE_PRIORITY__BITWISE,

    [SATISFIABLE_TOK__PERCENT]              = SATISFIABLE_PRIORITY__MOD,

    [SATISFIABLE_TOK__PLUS]                 = SATISFIABLE_PRIORITY__ARITHMETIC,
    [SATISFIABLE_TOK__MINUS]                = SATISFIABLE_PRIORITY__ARITHMETIC,

    [SATISFIABLE_TOK__STAR]                 = SATISFIABLE_PRIORITY__COMPLEX_ARITHMETIC,
    [SATISFIABLE_TOK__SLASH]                = SATISFIABLE_PRIORITY__COMPLEX_ARITHMETIC,

    [SATISFIABLE_TOK__COLON]                = SATISFIABLE_PRIORITY__TYPE,

    [SATISFIABLE_TOK__NEG]                  = SATISFIABLE_PRIORITY__UNARY,
    [SATISFIABLE_TOK__POS]                  = SATISFIABLE_PRIORITY__UNARY,
    [SATISFIABLE_TOK__TILDE]                = SATISFIABLE_PRIORITY__UNARY,

    [SATISFIABLE_TOK__PERIOD]               = SATISFIABLE_PRIORITY__ELEMENTOF,
    [SATISFIABLE_TOK__FIELDOF]              = SATISFIABLE_PRIORITY__ELEMENTOF
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
    return strspn(str, "+-*/%:;?&|^~!~=><()[]{},.\\");
}

// find position of terminator.
// if preceeded by '\', find next.
static size_t satisfiable_tok__strlen(const char* const restrict base, const char terminator){
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
static size_t satisfiable_tok__numlen(const char* const restrict str){
    if(str[0] == '"') return satisfiable_tok__strlen(str, '"');
    if(str[0] == '\'') return satisfiable_tok__strlen(str, '\'');
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
