#ifndef LTH_TOKENISER_TYPES_VTYPE_H_INCLUDED
#define LTH_TOKENISER_TYPES_VTYPE_H_INCLUDED

// tokenises value types.

#include "../../common/_common.h"       // error ctx.

// stdlib includes.
#include "stddef.h"                     // size_t, NULL.
#include "stdint.h"                     // int types.
#include "string.h"                     // strncmp.

// hash function, generated with vfasttables version 1.1.0.
static lth_nameT lth_vtype__hash(const char* str, const size_t len){
// associated values generated for hash function.
// hand-optimised B))).
    const uint8_t associated_values[] = {
        [2] = 4,
        [4] = 4,
        ['u'] = 0,
        ['i'] = 4,
        ['8'] = 0,
        ['1'] = 1,
        ['3'] = 2,
        ['6'] = 3,
    };
    if(len > 3) return 0;

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

static lth_nameT lth_vtype__search(const char* const restrict str, const size_t len){
    const lth_nameT index = lth_vtype__hash(str, len);
    char* table[12] = {
       [LTH_TYPE__U8]  = "u8",
       [LTH_TYPE__U16] = "u16",
       [LTH_TYPE__U32] = "u32",
       [LTH_TYPE__U64] = "u64",

       [LTH_TYPE__I8]  = "i8",
       [LTH_TYPE__I16] = "i16",
       [LTH_TYPE__I32] = "i32",
       [LTH_TYPE__I64] = "i64"
    };
    return table[index] && strncmp(table[index], str, len) == 0? index: LTH_TYPE__NONE;
}

// really it should be encoding (hex, decimal, octal, binary) + type (whatever :ppp).
static lth_nameT lth_vtype(
    const char* const restrict data, const size_t s_data,
    lth_error_ctxT* const restrict err, rcT* const restrict path, const lth_fposT line, const lth_fposT offset
){
    if(s_data >= 3){
        const size_t max = strcspn(data, "iu");
        if(max > s_data) goto SKIP_SEARCH;
        if(s_data - max > 2 || s_data - max == 0){
            lth_error_report(LTH_ERROR__VALUE_FORMAT_UNKNOWN, LTH_ERROR__COMPILER, path, line, offset, err);
            return LTH_TYPE__NONE;
        }
        const lth_nameT type = lth_vtype__search(data + s_data - 3, 3);
        if(type == LTH_TYPE__NONE)
            lth_error_report(LTH_ERROR__VALUE_FORMAT_UNKNOWN, LTH_ERROR__COMPILER, path, line, offset, err);
        return type;
    }
SKIP_SEARCH:
    const char* const restrict period = strchr(data, '.');
    if(period && period - data < s_data){
        if(data[s_data - 1] == 'f')
            return LTH_TYPE__F32;
        else return LTH_TYPE__F64;
    }
// default type for literals.
    return LTH_TYPE__I32;
}

#endif
