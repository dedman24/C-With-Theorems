#ifndef LTH_COMMON_VARIABLES_H_INCLUDED
#define LTH_COMMON_VARIABLES_H_INCLUDED

// handles variables, variable ctx.

#include "tok.h"                          // lth__representation_tokT.
#include "linking/vfastrht/vfastrht.h"    // vfastrhtT type & functions.


// stdlib includes.
#include "stddef.h"         // size_t, NULL.
#include "stdlib.h"         // malloc, free.

// some day you'll realise that everything here sux.
// some day we'll all be fine, consumed by the dust...

// types defined when initialising value ctx.
enum{
  LTH_TYPE__CHAR,
  LTH_TYPE__TYPE,
  LTH_TYPE__STRING,
  LTH_TYPE__BOOLEAN,

  LTH_TYPE__U8,
  LTH_TYPE__U16,
  LTH_TYPE__U32,
  LTH_TYPE__U64,

  LTH_TYPE__I8,
  LTH_TYPE__I16,
  LTH_TYPE__I32,
  LTH_TYPE__I64,

  LTH_TYPE__F8,
  LTH_TYPE__F16,
  LTH_TYPE__F32,
  LTH_TYPE__F64,

  LTH_TYPE__NONE,                         // used for error values.
  LTH_TYPE__UNDEFINED,                    // used for yet-to-be-defined variables.
};

// possibly avoid c-like type names?
const char* const restrict lth_type__strings[] = {
  [LTH_TYPE__CHAR]      = "char",
  [LTH_TYPE__TYPE]      = "type",
  [LTH_TYPE__STRING]    = "string",
  [LTH_TYPE__BOOLEAN]   = "bool",

  [LTH_TYPE__U8]        = "u8",
  [LTH_TYPE__U16]       = "u16",
  [LTH_TYPE__U32]       = "u32",
  [LTH_TYPE__U64]       = "u64",

  [LTH_TYPE__I8]        = "i8",
  [LTH_TYPE__I16]       = "i16",
  [LTH_TYPE__I32]       = "i32",
  [LTH_TYPE__I64]       = "i64",

  [LTH_TYPE__F8]        = "f8",
  [LTH_TYPE__F16]       = "f16",
  [LTH_TYPE__F32]       = "f32",
  [LTH_TYPE__F64]       = "f64",

  [LTH_TYPE__NONE]      = "ERROR",
  [LTH_TYPE__UNDEFINED] = "UNDEFINED"
};

// doesn't really work for user-defined qualifiers, but those are an abstract idea anyways.
typedef enum: uint8_t{
  LTH_QUALIFIER__NONE     = 0,
  LTH_QUALIFIER__FREENAME = 1<<0,         // cheeky hiding this in qualifierT, but it can lead to bugs (LORD forbid!). it saves on space, so...
  LTH_QUALIFIER__CONST    = 1<<1,
  LTH_QUALIFIER__ARRAY    = 1<<2
} lth_qualifierT;

typedef struct{
  void* internal;                         // internal representation/actual contents of variable.
  const char* name;                       // string name of variable.
  size_t s_name;                          // length of name in bytes.
  lth_nameT type;                         // value type.
  lth_qualifierT qualifier;               // qualifiers.
} lth_variableT;

static size_t lth_type__sizeof(lth_variableT* const restrict variable){
  const size_t sizes[] = {
    [LTH_TYPE__CHAR]    = 1,
    [LTH_TYPE__TYPE]    = 0,
    [LTH_TYPE__BOOLEAN] = 1,

    [LTH_TYPE__U8]      = 1,
    [LTH_TYPE__U16]     = 2,
    [LTH_TYPE__U32]     = 4,
    [LTH_TYPE__U64]     = 8,

    [LTH_TYPE__I8]      = 1,
    [LTH_TYPE__I16]     = 2,
    [LTH_TYPE__I32]     = 4,
    [LTH_TYPE__I64]     = 8,

    [LTH_TYPE__F8]      = 1,
    [LTH_TYPE__F16]     = 2,
    [LTH_TYPE__F32]     = 4,
    [LTH_TYPE__F64]     = 8
  };

  if(variable->type == LTH_TYPE__STRING) return strlen(variable->internal);
  return sizes[variable->type];
}

static lth_variableT* lth_variable_init(
  const char* const restrict name, const size_t s_name,
  void* const restrict internal, const lth_nameT type, const lth_qualifierT qualifier
){
  lth_variableT* const restrict var = malloc(sizeof(*var));

  var->internal = internal;
  var->name = name;
  var->s_name = s_name;
  var->type = type;
  var->qualifier = qualifier;

  return var;
}

static void lth_variable_destroy(vfastrht_objT* const restrict var){
  lth_variableT* const restrict real = (lth_variableT*)var;
  if(real->qualifier & LTH_QUALIFIER__FREENAME) free((void*)real->name);
  free(real->internal);
  free(real);
}

typedef struct{
  vfastrhtT* correspondences;
  lth_nameT id_name;
} lth_variable_ctxT;

static void lth_variable_ctx_destroy(lth_variable_ctxT* const restrict ctx, const bool freectx){
  vfastrht_table_destroy(ctx->correspondences, lth_variable_destroy);
  if(freectx) free(ctx);
}

static void lth_variable_add(
  lth_variable_ctxT* const restrict ctx,
  const lth_nameT id, const char* const restrict name, const size_t s_name,
  void* const restrict internal, const lth_nameT type, const lth_qualifierT qualifier
){
  lth_variableT* const restrict var = lth_variable_init(name, s_name, internal, type, qualifier);
  vfastrht_put(ctx->correspondences, id, var);
}

static lth_variableT* lth_variable_search(lth_variable_ctxT* const restrict ctx, const lth__underlying_tokT id){
  return vfastrht_search_elem(ctx->correspondences, id);
}

#define LTH_VARIABLE__ID_ERROR 1

static lth_variable_ctxT* lth_variable_ctx_init(lth_variable_ctxT* restrict ctx){
  if(!ctx) ctx = malloc(sizeof(*ctx));

  ctx->correspondences = vfastrht_table_init();
  ctx->id_name = 2;    // if set to 0, conflicts with vfastrht stuff. also, value of id 1 is reserved for an error value used during execution.

  lth_variable_add(ctx, LTH_VARIABLE__ID_ERROR, NULL, 0, NULL, LTH_TYPE__NONE, LTH_QUALIFIER__FREENAME);

  return ctx;
}

static lth_nameT lth_variable_name(lth_variable_ctxT* restrict ctx){
  return ctx->id_name++;
}

#endif
