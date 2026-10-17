#ifndef SATISFIABLE_COMMON_VARIABLES_H_INCLUDED
#define SATISFIABLE_COMMON_VARIABLES_H_INCLUDED

// handles variables, variable ctx.

#include "tok.h"                          // satisfiable__representation_tokT.
#include "linking/vfastrht/vfastrht.h"    // vfastrhtT type & functions.


// stdlib includes.
#include "stddef.h"         // size_t, NULL.
#include "stdlib.h"         // malloc, free.

typedef satisfiable__representation_tokT satisfiable_nameT;

// some day you'll realise that everything here sux.
// some day we'll all be fine, consumed by the dust...
typedef enum{
  SATISFIABLE_TYPE__U8 = 3,
  SATISFIABLE_TYPE__U16 = 6,
  SATISFIABLE_TYPE__U32 = 9,
  SATISFIABLE_TYPE__U64 = 0,

  SATISFIABLE_TYPE__I8 = 4,
  SATISFIABLE_TYPE__I16 = 7,
  SATISFIABLE_TYPE__I32 = 10,
  SATISFIABLE_TYPE__I64 = 1,

  SATISFIABLE_TYPE__F8 = 5,
  SATISFIABLE_TYPE__F16 = 8,
  SATISFIABLE_TYPE__F32 = 11,
  SATISFIABLE_TYPE__F64 = 2,

  SATISFIABLE_TYPE__NONE = 12,                    // used for error values.
  SATISFIABLE_TYPE__UNDEFINED,                    // used for yet-to-be-defined variables.

  SATISFIABLE_TYPE__CHAR,
  SATISFIABLE_TYPE__TYPE,
  SATISFIABLE_TYPE__STRING,
  SATISFIABLE_TYPE__BOOLEAN,
} satisfiable_typeT;

// possibly avoid c-like type names?
const char* const restrict satisfiable_type__strings[] = {
  [SATISFIABLE_TYPE__U8]        = "uint8_t",
  [SATISFIABLE_TYPE__U16]       = "uint16_t",
  [SATISFIABLE_TYPE__U32]       = "uint32_t",
  [SATISFIABLE_TYPE__U64]       = "uint64_t",

  [SATISFIABLE_TYPE__I8]        = "int8_t",
  [SATISFIABLE_TYPE__I16]       = "int16_t",
  [SATISFIABLE_TYPE__I32]       = "int32_t",
  [SATISFIABLE_TYPE__I64]       = "int64_t",

  [SATISFIABLE_TYPE__F8]        = "float8_t",
  [SATISFIABLE_TYPE__F16]       = "float16_t",
  [SATISFIABLE_TYPE__F32]       = "float32_t",
  [SATISFIABLE_TYPE__F64]       = "float64_t",

  [SATISFIABLE_TYPE__NONE]      = "ERROR",
  [SATISFIABLE_TYPE__UNDEFINED] = "UNDEFINED",

  [SATISFIABLE_TYPE__CHAR]      = "char",
  [SATISFIABLE_TYPE__TYPE]      = "type_t",
  [SATISFIABLE_TYPE__STRING]    = "string",
  [SATISFIABLE_TYPE__BOOLEAN]   = "bool"
};

// doesn't really work for user-defined qualifiers, but those are an abstract idea anyways.
typedef enum: uint8_t{
  SATISFIABLE_QUALIFIER__NONE     = 0,
  SATISFIABLE_QUALIFIER__FREENAME = 1<<0,         // cheeky hiding this in qualifierT, but it can lead to bugs (LORD forbid!). it saves on space, so...
  SATISFIABLE_QUALIFIER__CONST    = 1<<1,
  SATISFIABLE_QUALIFIER__ARRAY    = 1<<2
} satisfiable_qualifierT;

typedef struct{
  void* internal;                                 // internal representation/actual contents of variable.
  const char* name;                               // string name of variable.
  size_t s_name;                                  // length of name in bytes.
  satisfiable_typeT type;                         // value type.
  satisfiable_qualifierT qualifier;               // qualifiers.
} satisfiable_variableT;

static satisfiable_variableT* satisfiable_variable_init(
  const char* const restrict name, const size_t s_name,
  void* const restrict internal, const satisfiable_typeT type, const satisfiable_qualifierT qualifier
){
  satisfiable_variableT* const restrict var = malloc(sizeof(*var));

  var->internal = internal;
  var->name = name;
  var->s_name = s_name;
  var->type = type;
  var->qualifier = qualifier;

  return var;
}

static void satisfiable_variable_destroy(vfastrht_objT* const restrict var){
  satisfiable_variableT* const restrict real = (satisfiable_variableT*)var;
  if(real->qualifier & SATISFIABLE_QUALIFIER__FREENAME) free((void*)real->name);
  free(real);
}

typedef struct{
  vfastrhtT* correspondences;
  satisfiable_nameT id_name;
} satisfiable_variable_ctxT;

static void satisfiable_variable_ctx_destroy(satisfiable_variable_ctxT* const restrict ctx, const bool freectx){
  vfastrht_table_destroy(ctx->correspondences, satisfiable_variable_destroy);
  if(freectx) free(ctx);
}

static void satisfiable_variable_add(
  satisfiable_variable_ctxT* const restrict ctx,
  const satisfiable_nameT id, const char* const restrict name, const size_t s_name,
  void* const restrict internal, const satisfiable_typeT type, const satisfiable_qualifierT qualifier
){
  satisfiable_variableT* const restrict var = satisfiable_variable_init(name, s_name, internal, type, qualifier);
  vfastrht_put(ctx->correspondences, id, var);
}

static satisfiable_variableT* satisfiable_variable_search(satisfiable_variable_ctxT* const restrict ctx, const satisfiable__underlying_tokT id){
  return vfastrht_search_elem(ctx->correspondences, id);
}

#define SATISFIABLE_VARIABLE__ID_ERROR 1

static satisfiable_variable_ctxT* satisfiable_variable_ctx_init(satisfiable_variable_ctxT* restrict ctx){
  if(!ctx) ctx = malloc(sizeof(*ctx));

  ctx->correspondences = vfastrht_table_init();
  ctx->id_name = 2;    // if set to 0, conflicts with vfastrht stuff. also, value of id 1 is reserved for an error value used during execution.

  satisfiable_variable_add(ctx, SATISFIABLE_VARIABLE__ID_ERROR, NULL, 0, NULL, SATISFIABLE_TYPE__NONE, SATISFIABLE_QUALIFIER__FREENAME);

  return ctx;
}

static satisfiable_nameT satisfiable_variable_name(satisfiable_variable_ctxT* restrict ctx){
  return ctx->id_name++;
}

#endif
