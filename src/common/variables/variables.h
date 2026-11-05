#ifndef CTH_COMMON_VARIABLES_H_INCLUDED
#define CTH_COMMON_VARIABLES_H_INCLUDED

// handles variables, variable ctx.

#include "ctx.h"                            // important data types.
#include "types.h"                          // type handling.
#include "../tok.h"                         // cth__representation_tokT.
#include "../linking/vfastarr.h"            // vfastarr functions.
#include "../linking/vfastrht/vfastrht.h"   // vfastrhtT type & functions.

#include "../../interpreter/ast.h"          // abstract syntax tree, for type handling. allows arbitrarily complex propositions as types.

// stdlib includes.
#include "stddef.h"         // size_t, NULL.
#include "stdlib.h"         // malloc, free.

// some day you'll realise that everything here sux.
// some day we'll all be fine, consumed by the dust...

static cth_variable_stateT cth_qualifier_op2(const cth_variable_stateT q0, const cth_variable_stateT q1){
  return q0 & q1;
}

// returns 0 on invalid queries.
static size_t cth_type__sizeof(cth_variableT* const restrict variable){
  const size_t sizes[] = {
    [CTH_TYPE__CHAR8]         = sizeof(char),
    [CTH_TYPE__CHAR16]        = sizeof(char16_t),
    [CTH_TYPE__CHAR32]        = sizeof(char32_t),

    [CTH_TYPE__U8]            = 1,
    [CTH_TYPE__U16]           = 2,
    [CTH_TYPE__U32]           = 4,
    [CTH_TYPE__U64]           = 8,

    [CTH_TYPE__I8]            = 1,
    [CTH_TYPE__I16]           = 2,
    [CTH_TYPE__I32]           = 4,
    [CTH_TYPE__I64]           = 8,

    [CTH_TYPE__F8]            = 1,
    [CTH_TYPE__F16]           = 2,
    [CTH_TYPE__F32]           = 4,
    [CTH_TYPE__F64]           = 8,

    [CTH_TYPE__CHAR]          = sizeof(char),
    [CTH_TYPE__SCHAR]         = sizeof(signed char),
    [CTH_TYPE__UCHAR]         = sizeof(unsigned char),
    [CTH_TYPE__SHORT]         = sizeof(short),
    [CTH_TYPE__USHORT]        = sizeof(unsigned short),
    [CTH_TYPE__INT]           = sizeof(int),
    [CTH_TYPE__UINT]          = sizeof(unsigned int),
    [CTH_TYPE__LONG]          = sizeof(long),
    [CTH_TYPE__ULONG]         = sizeof(unsigned long),
    [CTH_TYPE__LONG_LONG]     = sizeof(long long),
    [CTH_TYPE__ULONG_LONG]    = sizeof(unsigned long long),
    [CTH_TYPE__FLOAT]         = sizeof(float),
    [CTH_TYPE__DOUBLE]        = sizeof(double),
    [CTH_TYPE__LONG_DOUBLE]   = sizeof(long double),

    [CTH_TYPE__SIGNED]        = sizeof(signed),
    [CTH_TYPE__UNSIGNED]      = sizeof(unsigned),

    [CTH_TYPE__BOOLEAN]       = sizeof(bool),
    [CTH_TYPE__TYPE]          = 0,
    [CTH_TYPE__QUALIFIER]     = 0,
    
    [CTH_TYPE__SSIZE]         = sizeof(ssize_t),
    [CTH_TYPE__SIZE]          = sizeof(size_t),
  };

  if(variable->type == CTH_TYPE__STRING) return strlen(variable->internal.string);
  return sizes[variable->type];
}

// tries to combine C types.
// very ugly, but nothing else can be done.
static cth_nameT cth_type__combine(const cth_nameT t0, const cth_nameT t1){
  switch(t0){
    case CTH_TYPE__SHORT: 
      return t1 == CTH_TYPE__INT? 
        CTH_TYPE__SHORT:
        CTH_TYPE__NONE;
    case CTH_TYPE__LONG:
      switch(t1){
        case CTH_TYPE__INT:    return CTH_TYPE__LONG;           // long int == long.
        case CTH_TYPE__LONG:   return CTH_TYPE__LONG_LONG;      // long long != long.
        case CTH_TYPE__DOUBLE: return CTH_TYPE__LONG_DOUBLE;    // long double != long.
        default: return CTH_TYPE__NONE;
      }
    case CTH_TYPE__LONG_LONG:
      return t1 == CTH_TYPE__INT?
        CTH_TYPE__LONG_LONG:
        CTH_TYPE__NONE;
    case CTH_TYPE__SIGNED:
      switch(t1){
        case CTH_TYPE__U8:  return CTH_TYPE__I8;
        case CTH_TYPE__U16: return CTH_TYPE__I16;
        case CTH_TYPE__U32: return CTH_TYPE__I32;
        case CTH_TYPE__U64: return CTH_TYPE__I64;
        case CTH_TYPE__CHAR: return CTH_TYPE__SCHAR;
        case CTH_TYPE__SIZE: return CTH_TYPE__SSIZE;

        case CTH_TYPE__SHORT:
        case CTH_TYPE__INT: 
        case CTH_TYPE__LONG:
        case CTH_TYPE__LONG_LONG:
          return t1;

        default:
          return CTH_TYPE__NONE;
      }
    case CTH_TYPE__UNSIGNED:
      switch(t1){
        case CTH_TYPE__I8:  return CTH_TYPE__U8;
        case CTH_TYPE__I16: return CTH_TYPE__U16;
        case CTH_TYPE__I32: return CTH_TYPE__U32;
        case CTH_TYPE__I64: return CTH_TYPE__U64;
        case CTH_TYPE__CHAR: return CTH_TYPE__UCHAR;

        case CTH_TYPE__SHORT:
        case CTH_TYPE__INT: 
        case CTH_TYPE__LONG:
        case CTH_TYPE__LONG_LONG:
        case CTH_TYPE__SSIZE:
          return t1 + 2;          // for these types, 'type + 2' returns the unsigned equivalent. quite unsafe but very fast.

        default:
          return CTH_TYPE__NONE;
      }
    default: return CTH_TYPE__NONE;
  }
}

// does not initialise 'internal' field.
static cth_variableT* cth_variable_init(
  const char* const restrict name, const size_t s_name,
  const cth_nameT type, const cth_variable_stateT qualifier
){
  cth_variableT* const restrict var = malloc(sizeof(*var));

  var->name = name;
  var->s_name = s_name;
  var->type = type;
  var->state = qualifier;

  return var;
}

static void cth_variable_destroy(cth_variableT* const restrict real){
  if(real->state & CTH_VARSTATE__FREENAME) free((void*)real->name);
  switch(real->type){
    case CTH_TYPE__TYPE:
      cth_type__internal_destroy(real->internal.type);
      break;
    case CTH_TYPE__STRING:
      free(real->internal.string);
      break;
    default: break;
  }
  free(real);
}

static void cth_variable_destroy__rht(vfastrht_objT* const restrict var){
  cth_variableT* const restrict real = (cth_variableT*)var;
  if(real->state & CTH_VARSTATE__FREENAME) free((void*)real->name);
  switch(real->type){
    case CTH_TYPE__TYPE: 
      cth_type__internal_destroy(real->internal.type);
      break;
    case CTH_TYPE__STRING:
      free(real->internal.string);
      break;
    default: break;
  }
  free(real);
}

static void cth_variable__internal_clone(cth_variableT* const restrict dst, cth_variableT* const restrict src){
  switch(dst->type){
    case CTH_TYPE__TYPE:
      dst->internal.type = cth_type__internal_acquire(src->internal.type);
      break;
    case CTH_TYPE__STRING:
      const size_t s_str = strlen(src->internal.string) + 1;
      dst->internal.string = malloc(s_str);
      memcpy(dst->internal.string, src->internal.string, s_str);
      break;
    default:
      dst->internal = src->internal;
      break;
  }  
}

typedef struct{
  vfastrhtT* correspondences;
  cth_nameT id_name;
} cth_variable_ctxT;

static cth_nameT cth_variable_name(cth_variable_ctxT* restrict ctx){
  return ctx->id_name += 2;
}

static void cth_variable_ctx_destroy(cth_variable_ctxT* const restrict ctx, const bool freectx){
  vfastrht_table_destroy(ctx->correspondences, cth_variable_destroy__rht);
  if(freectx) free(ctx);
}

static cth_variableT* cth_variable_add(
  cth_variable_ctxT* const restrict ctx,
  const cth_nameT id, const char* const restrict name, const size_t s_name,
  const cth_nameT type, const cth_variable_stateT state
){
  cth_variableT* const restrict var = cth_variable_init(name, s_name, type, state);
  vfastrht_put(ctx->correspondences, id, var);
  return var;
}

static cth_variableT* cth_variable_search(cth_variable_ctxT* const restrict ctx, const cth_nameT id){
  return vfastrht_search_elem(ctx->correspondences, id);
}

// TODO: add initialising all kinds of stuff here.
static cth_variable_ctxT* cth_variable_ctx_init(cth_variable_ctxT* restrict ctx){
  if(!ctx) ctx = malloc(sizeof(*ctx));

  ctx->correspondences = vfastrht_table_init();
  ctx->id_name = CTH_QUALIFIER_MAX | 1;          // must be odd.

  for(cth_nameT i = CTH_TYPE_MIN; i < CTH_TYPE_MAX; i += 2){
  // HAS to initialise `internal` field with AST node.
    cth_variableT* const restrict type = cth_variable_add(ctx, i, NULL, 0, CTH_TYPE__TYPE, CTH_VARSTATE__FREENAME);
    type->internal.type = cth_type__internal_init__id(i);

  }
  for(cth_nameT i = CTH_QUALIFIER_MIN; i < CTH_QUALIFIER_MAX; i += 2)
    cth_variable_add(ctx, i, NULL, 0, CTH_TYPE__QUALIFIER, CTH_VARSTATE__FREENAME);

  return ctx;
}

// returns NULL-terminated string holding fully qualified type name.
// has to be destroyed with vfastarr.
static char* cth_type_str_construct(const cth_astT* type, cth_variable_ctxT* const restrict variables, size_t* s_type_name){
  char* type_name = vfastarr_init(sizeof(char));

  while(1){
    switch(type->type){
      case CTH_AST__ID:
        if(type->data.id < CTH_TYPE_MAX)
          vfastarr_append_str(type_name, cth_type__strings[type->data.id]);
        else{                       // variable is defined from other variable, so we have to use that name.
          cth_variableT* const restrict v = cth_variable_search(variables, type->data.id);
          vfastarr_append(type_name, v->name, v->s_name);
        }
        *s_type_name = vfastarr_size(type_name);
        vfastarr_append_char(type_name, '\0');
        return type_name;
      case CTH_AST__FUNCALL:
        cth_astT* const restrict fun = type->data._funcall.fun;
      // NOTE: supports unary qualifiers like *, immut, const but not binary ones like arrays (or even function types TwT).
      // upgrade code for when the type comes to add those.
        vfastarr_append_str(type_name, cth_qualifier__strings[fun->data.id]);
        vfastarr_append_char(type_name, ' ');
        type = type->data._funcall.args[0];
        break;
      default:      // should be impossible to arrive here, but I CBA to report an error, so we force a segfault :PPP.
        *(volatile uint8_t*)NULL = 67;
        break;
    }
  }
};

#endif
