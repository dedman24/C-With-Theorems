#ifndef CTH_COMMON_VARIABLES_H_INCLUDED
#define CTH_COMMON_VARIABLES_H_INCLUDED

// handles variables, variable ctx.

#include "ctx.h"                            // important data types.
#include "types.h"                          // type handling.
#include "basics.h"                         // basic operations.
#include "../tok.h"                         // cth__representation_tokT.
#include "../linking/rc.h"                  // reference-counted strings.
#include "../linking/vfastarr.h"            // vfastarr functions.

#include "../../interpreter/ast.h"          // abstract syntax tree, for type handling. allows arbitrarily complex propositions as types.

// stdlib includes.
#include "stddef.h"         // size_t, NULL.
#include "stdlib.h"         // malloc, free.

// some day you'll realise that everything here sux.
// some day we'll all be fine, consumed by the dust...

static cth_variable_stateT cth_qualifier_op2(const cth_variable_stateT q0, const cth_variable_stateT q1){
  return q0 & q1;
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
    case CTH_TYPE__ULONG:
      switch(t1){
        case CTH_TYPE__INT:    return CTH_TYPE__ULONG;          // unsigned long int == unsigned long.
        case CTH_TYPE__LONG:   return CTH_TYPE__ULONG_LONG;     // unsigned long long != unsigned long.
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
  cth_variableT* const type, const cth_variable_stateT qualifier
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
  if(real->type == cth_typevar(CTH_TYPE__TYPE))
    cth_type__internal_destroy(real->internal.type);
  else if(real->type == cth_typevar(CTH_TYPE__STRING))
    rc_disown(real->internal.string);
  free(real);
}

static void cth_variable_destroy__rht(vfastrht_objT* const restrict var){
  cth_variableT* const restrict real = (cth_variableT*)var;
  if(real->state & CTH_VARSTATE__FREENAME) free((void*)real->name);
  if(real->type == cth_typevar(CTH_TYPE__TYPE))
    cth_type__internal_destroy(real->internal.type);
  else if(real->type == cth_typevar(CTH_TYPE__STRING))
    rc_disown(real->internal.string);
  free(real);
}

static void cth_variable__internal_clone(cth_variableT* const restrict dst, cth_variableT* const restrict src){
  if(dst->type == cth_typevar(CTH_TYPE__TYPE))
    dst->internal.type = cth_type__internal_acquire(src->internal.type);
  else if(dst->type == cth_typevar(CTH_TYPE__STRING))
    dst->internal.string = rc_own(src->internal.string);
  else
    dst->internal = src->internal;
}

static cth_nameT cth_variable_name(cth_variable_ctxT* restrict ctx){
  return ctx->id_name += 2;
}

static void cth_variable_ctx_destroy(cth_variable_ctxT* const restrict ctx, const bool freectx){
  vfastrht_table_destroy(ctx->correspondences, cth_variable_destroy__rht);
  if(freectx) free(ctx);
}

// does not initialise 'internal' field.
static cth_variableT* cth_variable_add(
  cth_variable_ctxT* const restrict ctx,
  const cth_nameT id, const char* const restrict name, const size_t s_name,
  cth_variableT* const type, const cth_variable_stateT state
){
  cth_variableT* const restrict var = cth_variable_init(name, s_name, type, state);
  vfastrht_put(ctx->correspondences, id, var);
  return var;
}

static cth_variableT* cth_variable_search(cth_variable_ctxT* const restrict ctx, const cth_nameT id){
  return vfastrht_search_elem(ctx->correspondences, id);
}

static void cth_variable_ctx_init__type(cth_variable_ctxT* restrict ctx){
  cth_variableT* const restrict type = cth_variable_add(ctx, CTH_TYPE__TYPE, NULL, 0, NULL, CTH_VARSTATE__NONE);
  type->type = type;
  type->internal.type = cth_type__internal_init__id(CTH_TYPE__TYPE);
  cth_type__var_arr[CTH_TYPE__TYPE/2] = type;

  for(size_t i = CTH_TYPE_MIN; i < CTH_TYPE_MAX; i+=2){
    if(i == CTH_TYPE__TYPE) continue;
    cth_type__var_arr[i/2] = cth_variable_add(ctx, i, NULL, 0, type, CTH_VARSTATE__NONE);
    cth_type__var_arr[i/2]->internal.type = cth_type__internal_init__id(i);
  }
}

// TODO: add initialising all kinds of stuff here.
static cth_variable_ctxT* cth_variable_ctx_init(cth_variable_ctxT* restrict ctx){
  if(!ctx) ctx = malloc(sizeof(*ctx));

  ctx->correspondences = vfastrht_table_init();
  ctx->id_name = CTH_NAME__ID_START;
// order is important; ctx_init__type MUST be ahead of any other init function.
  cth_variable_ctx_init__type(ctx);
  cth_variable_ctx_init__basic(ctx);
  for(cth_nameT i = CTH_QUALIFIER_MIN; i < CTH_QUALIFIER_MAX; i += 2)
    cth_variable_add(ctx, i, NULL, 0, cth_typevar(CTH_TYPE__QUALIFIER), CTH_VARSTATE__FREENAME);
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
          type_name = vfastarr_append_str(type_name, cth_type__strings[type->data.id]);
        else{                       // variable is defined from other variable, so we have to use that name.
          cth_variableT* const restrict v = cth_variable_search(variables, type->data.id);
          type_name = vfastarr_append(type_name, v->name, v->s_name);
        }
        *s_type_name = vfastarr_size(type_name);
        type_name = vfastarr_append_char(type_name, '\0');
        return type_name;
      case CTH_AST__FUNCALL:
        cth_astT* const restrict fun = type->data._funcall.fun;
      // NOTE: supports unary qualifiers like *, immut, const but not binary ones like arrays (or even function types TwT).
      // upgrade code for when the type comes to add those.
        type_name = vfastarr_append_str(type_name, cth_qualifier__strings[fun->data.id]);
        type_name = vfastarr_append_char(type_name, ' ');
        type = type->data._funcall.args[0];
        break;
      default:      // should be impossible to arrive here, but I CBA to report an error, so we force a segfault :PPP.
        *(volatile uint8_t*)NULL = 67;
        break;
    }
  }
};

#endif
