#ifndef CTH_COMMON_VARIABLES_H_INCLUDED
#define CTH_COMMON_VARIABLES_H_INCLUDED

// handles variables, variable ctx.

#include "tok.h"                          // cth__representation_tokT.
#include "linking/vfastrht/vfastrht.h"    // vfastrhtT type & functions.


// stdlib includes.
#include "stddef.h"         // size_t, NULL.
#include "stdlib.h"         // malloc, free.

// some day you'll realise that everything here sux.
// some day we'll all be fine, consumed by the dust...

// types defined when initialising value ctx.
enum{
  CTH_TYPE__CHAR,
  CTH_TYPE__TYPE,
  CTH_TYPE__STRING,
  CTH_TYPE__BOOLEAN,

  CTH_TYPE__U8,
  CTH_TYPE__U16,
  CTH_TYPE__U32,
  CTH_TYPE__U64,

  CTH_TYPE__I8,
  CTH_TYPE__I16,
  CTH_TYPE__I32,
  CTH_TYPE__I64,

  CTH_TYPE__F8,
  CTH_TYPE__F16,
  CTH_TYPE__F32,
  CTH_TYPE__F64,

  CTH_TYPE__NONE,                         // used for error values.
  CTH_TYPE__UNDEFINED,                    // used for yet-to-be-defined variables.
};

// possibly avoid c-like type names?
const char* const restrict cth_type__strings[] = {
  [CTH_TYPE__CHAR]      = "char",
  [CTH_TYPE__TYPE]      = "type",
  [CTH_TYPE__STRING]    = "string",
  [CTH_TYPE__BOOLEAN]   = "bool",

  [CTH_TYPE__U8]        = "u8",
  [CTH_TYPE__U16]       = "u16",
  [CTH_TYPE__U32]       = "u32",
  [CTH_TYPE__U64]       = "u64",

  [CTH_TYPE__I8]        = "i8",
  [CTH_TYPE__I16]       = "i16",
  [CTH_TYPE__I32]       = "i32",
  [CTH_TYPE__I64]       = "i64",

  [CTH_TYPE__F8]        = "f8",
  [CTH_TYPE__F16]       = "f16",
  [CTH_TYPE__F32]       = "f32",
  [CTH_TYPE__F64]       = "f64",

  [CTH_TYPE__NONE]      = "ERROR",
  [CTH_TYPE__UNDEFINED] = "UNDEFINED"
};

// doesn't really work for user-defined qualifiers, but those are an abstract idea anyways.
typedef enum: uint8_t{
  CTH_QUALIFIER__NONE     = 0,
  CTH_QUALIFIER__FREENAME = 1<<0,         // cheeky hiding this in qualifierT, but it can lead to bugs (LORD forbid!). it saves on space, so...
  CTH_QUALIFIER__CONST    = 1<<1,
  CTH_QUALIFIER__ARRAY    = 1<<2
} cth_qualifierT;

typedef struct{
  void* internal;                         // internal representation/actual contents of variable.
  const char* name;                       // string name of variable.
  size_t s_name;                          // length of name in bytes.
  cth_nameT type;                         // value type.
  cth_qualifierT qualifier;               // qualifiers.
} cth_variableT;

static size_t cth_type__sizeof(cth_variableT* const restrict variable){
  const size_t sizes[] = {
    [CTH_TYPE__CHAR]    = 1,
    [CTH_TYPE__TYPE]    = 0,
    [CTH_TYPE__BOOLEAN] = 1,

    [CTH_TYPE__U8]      = 1,
    [CTH_TYPE__U16]     = 2,
    [CTH_TYPE__U32]     = 4,
    [CTH_TYPE__U64]     = 8,

    [CTH_TYPE__I8]      = 1,
    [CTH_TYPE__I16]     = 2,
    [CTH_TYPE__I32]     = 4,
    [CTH_TYPE__I64]     = 8,

    [CTH_TYPE__F8]      = 1,
    [CTH_TYPE__F16]     = 2,
    [CTH_TYPE__F32]     = 4,
    [CTH_TYPE__F64]     = 8
  };

  if(variable->type == CTH_TYPE__STRING) return strlen(variable->internal);
  return sizes[variable->type];
}

static cth_variableT* cth_variable_init(
  const char* const restrict name, const size_t s_name,
  void* const restrict internal, const cth_nameT type, const cth_qualifierT qualifier
){
  cth_variableT* const restrict var = malloc(sizeof(*var));

  var->internal = internal;
  var->name = name;
  var->s_name = s_name;
  var->type = type;
  var->qualifier = qualifier;

  return var;
}

static void cth_variable_destroy(vfastrht_objT* const restrict var){
  cth_variableT* const restrict real = (cth_variableT*)var;
  if(real->qualifier & CTH_QUALIFIER__FREENAME) free((void*)real->name);
  free(real->internal);
  free(real);
}

typedef struct{
  vfastrhtT* correspondences;
  cth_nameT id_name;
} cth_variable_ctxT;

static void cth_variable_ctx_destroy(cth_variable_ctxT* const restrict ctx, const bool freectx){
  vfastrht_table_destroy(ctx->correspondences, cth_variable_destroy);
  if(freectx) free(ctx);
}

static void cth_variable_add(
  cth_variable_ctxT* const restrict ctx,
  const cth_nameT id, const char* const restrict name, const size_t s_name,
  void* const restrict internal, const cth_nameT type, const cth_qualifierT qualifier
){
  cth_variableT* const restrict var = cth_variable_init(name, s_name, internal, type, qualifier);
  vfastrht_put(ctx->correspondences, id, var);
}

static cth_variableT* cth_variable_search(cth_variable_ctxT* const restrict ctx, const cth__underlying_tokT id){
  return vfastrht_search_elem(ctx->correspondences, id);
}

#define CTH_VARIABLE__ID_ERROR 1

// TODO: add initialising all kinds of stuff here.
static cth_variable_ctxT* cth_variable_ctx_init(cth_variable_ctxT* restrict ctx){
  if(!ctx) ctx = malloc(sizeof(*ctx));

  ctx->correspondences = vfastrht_table_init();
  ctx->id_name = 2;    // if set to 0, conflicts with vfastrht stuff. also, value of id 1 is reserved for an error value used during execution.

  cth_variable_add(ctx, CTH_VARIABLE__ID_ERROR, NULL, 0, NULL, CTH_TYPE__NONE, CTH_QUALIFIER__FREENAME);

  return ctx;
}

static cth_nameT cth_variable_name(cth_variable_ctxT* restrict ctx){
  return ctx->id_name++;
}

#endif
