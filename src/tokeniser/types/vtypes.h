#ifndef SATISFIABLE_TOKENISER_TYPES_VTYPE_H_INCLUDED
#define SATISFIABLE_TOKENISER_TYPES_VTYPE_H_INCLUDED

// tokenises value types.

#include "../../common/_common.h"       // error ctx.

// stdlib includes.
#include "stddef.h"                     // size_t, NULL.
#include "stdint.h"                     // int types.
#include "string.h"                     // strncmp.

// hash function, generated with vfasttables version 1.1.0.
static satisfiable_typeT satisfiable_vtype__hash(const char* str, const size_t len){
    if(len > 127) return 0;
// associated values generated for hash function.
    const uint8_t associated_values[] = {
        84,88,46,239,56,96,187,19,97,195,133,240,255,13,163,150,
        164,205,16,29,80,114,33,248,79,152,160,170,223,209,247,63,
        106,164,42,243,107,130,94,113,95,22,98,228,215,185,43,151,
        196,144,36,159,99,63,78,184,150,0,213,166,114,226,77,225,
        174,151,7,6,95,33,50,207,117,135,179,195,83,51,60,99,
        132,220,17,233,148,54,27,235,243,98,61,31,162,184,38,239,
        111,199,146,243,122,239,197,201,18,196,167,114,25,253,35,110,
        209,243,3,108,58,255,6,67,213,55,40,109,212,69,105,27
    };
    const size_t indices[] = {
        1, 0
    };
    uint8_t hash = associated_values[len];
    for(size_t i = 0; i < sizeof(indices)/sizeof(*indices); i++){
        hash += str[associated_values[indices[i]] % len];
    }
    hash %= 12;
    return hash;
}

static satisfiable_typeT satisfiable_vtype__search(const char* const restrict str, const size_t len){
    const satisfiable_typeT index = satisfiable_vtype__hash(str, len);
    char* table[12] = {
       [SATISFIABLE_TYPE__I64] = "i64",
       [SATISFIABLE_TYPE__U16] = "u16",
       [SATISFIABLE_TYPE__I32] = "i32",
       [SATISFIABLE_TYPE__U64] = "u64",
       [SATISFIABLE_TYPE__U8]  = "u8",
       [SATISFIABLE_TYPE__I8]  = "i8",
       [SATISFIABLE_TYPE__U32] = "u32",
       [SATISFIABLE_TYPE__I16] = "i16"
    };
    return table[index] && strncmp(table[index], str, len) == 0? index: SATISFIABLE_TYPE__NONE;
}

// really it should be encoding (hex, decimal, octal, binary) + type (whatever :ppp).
static satisfiable_typeT satisfiable_vtype(
    const char* const restrict data, const size_t s_data,
    satisfiable_error_ctxT* const restrict err, rcT* const restrict path, const satisfiable_fposT line, const satisfiable_fposT offset
){
    if(data[0] == '"'){
    // assumes string is all until '\0' eof terminator.
        if(data[s_data - 1] != '"')
            satisfiable_error_report(SATISFIABLE_ERROR__VALUE_STRING_UNFINISHED, SATISFIABLE_ERROR__COMPILER, path, line, offset, err);
        return SATISFIABLE_TYPE__STRING;
    }
    if(data[0] == '\''){
        if(data[s_data - 1] != '\'')
            satisfiable_error_report(SATISFIABLE_ERROR__VALUE_CHAR_UNFINISHED, SATISFIABLE_ERROR__COMPILER, path, line, offset, err);
        if(s_data != 3)
            satisfiable_error_report(SATISFIABLE_ERROR__VALUE_CHAR_TOO_LONG, SATISFIABLE_ERROR__COMPILER, path, line, offset, err);
        return SATISFIABLE_TYPE__CHAR;
    }

    if(s_data >= 3){
        const size_t max = strcspn(data, "iu");
        if(max > s_data) goto SKIP_SEARCH;
        if(s_data - max > 2 || s_data - max == 0){
            satisfiable_error_report(SATISFIABLE_ERROR__VALUE_FORMAT_UNKNOWN, SATISFIABLE_ERROR__COMPILER, path, line, offset, err);
            return SATISFIABLE_TYPE__NONE;
        }
        const satisfiable_typeT vtype = satisfiable_vtype__search(data + s_data - 3, 3);
        if(vtype == SATISFIABLE_TYPE__NONE)
            satisfiable_error_report(SATISFIABLE_ERROR__VALUE_FORMAT_UNKNOWN, SATISFIABLE_ERROR__COMPILER, path, line, offset, err);
        return vtype;
    }
SKIP_SEARCH:
    const char* const restrict period = strchr(data, '.');
    if(period && period - data < s_data){
        if(data[s_data - 1] == 'f')
            return SATISFIABLE_TYPE__F32;
        else return SATISFIABLE_TYPE__F64;
    }
// default type for literals.
    return SATISFIABLE_TYPE__I32;
}

#endif
