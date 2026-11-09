#ifndef CTH_COMMON_VARIABLES_DEFINITIONS_H_INCLUDED
#define CTH_COMMON_VARIABLES_DEFINITIONS_H_INCLUDED

#include "ctx.h"                    // variable contexts.
#include "../tok.h"                 // nameT & frens.
#include "../linking/vfastarr.h"    // vfastarr stuff.
#include "../../interpreter/ast.h"  // abstract syntax tree.

// stdlib includes.
#include "string.h"                 // strlen.

// types defined when initialising value ctx.
enum{
// all must be odd.
  CTH_TYPE_MIN            = 1,                  // all predefined CTH types are in the interval [CTH_TYPE_MIN, CTH_TYPE_MAX).
  CTH_TYPE__CHAR8         = 1,                  // starts from 1 or otherwise encounters issues.
  CTH_TYPE__CHAR16        = 3,
  CTH_TYPE__CHAR32        = 7,
  CTH_TYPE__STRING        = 5,

  CTH_TYPE__U8            = 9,
  CTH_TYPE__U16           = 11,
  CTH_TYPE__U32           = 13,
  CTH_TYPE__U64           = 15,

  CTH_TYPE__I8            = 17,
  CTH_TYPE__I16           = 19,
  CTH_TYPE__I32           = 21,
  CTH_TYPE__I64           = 23,

  CTH_TYPE__F8            = 25,
  CTH_TYPE__F16           = 27,
  CTH_TYPE__F32           = 29,
  CTH_TYPE__F64           = 31,
// simple C types.
  CTH_TYPE__CHAR          = 33,
  CTH_TYPE__SCHAR         = 35,
  CTH_TYPE__UCHAR         = 37,
  CTH_TYPE__SHORT         = 39,
  CTH_TYPE__USHORT        = 41,
  CTH_TYPE__INT           = 43,
  CTH_TYPE__UINT          = 45,
  CTH_TYPE__LONG          = 47,
  CTH_TYPE__ULONG         = 49,
  CTH_TYPE__LONG_LONG     = 51,
  CTH_TYPE__ULONG_LONG    = 53,
  CTH_TYPE__FLOAT         = 55,
  CTH_TYPE__DOUBLE        = 57,
  CTH_TYPE__LONG_DOUBLE   = 59,

  CTH_TYPE__SSIZE         = 61,
  CTH_TYPE__SIZE          = 63,

  CTH_TYPE__SIGNED        = 65,
  CTH_TYPE__UNSIGNED      = 67,

  CTH_TYPE__BOOLEAN       = 69,
  CTH_TYPE__TYPE          = 71,
  CTH_TYPE__QUALIFIER     = 73,
  CTH_TYPE__NULLPTR       = 75,

  CTH_TYPE__NONE          = 77,

  CTH_TYPE_MAX            = 79,
};

// 'bool' considered integer type for C compatibility.
static bool cth_type__isint(const cth_nameT type){
  return
    type >= CTH_TYPE__CHAR8 && type <= CTH_TYPE__CHAR32 ||
    type >= CTH_TYPE__U8    && type <= CTH_TYPE__I64    ||
    type >= CTH_TYPE__CHAR  && type <= CTH_TYPE__BOOLEAN;
    
}

// possibly avoid c-like type names?
const char* const restrict cth_type__strings[] = {
  [CTH_TYPE__CHAR8]         = "char8_t",
  [CTH_TYPE__CHAR16]        = "char16_t",
  [CTH_TYPE__CHAR32]        = "char32_t",
  [CTH_TYPE__STRING]        = "string",

  [CTH_TYPE__U8]            = "u8",
  [CTH_TYPE__U16]           = "u16",
  [CTH_TYPE__U32]           = "u32",
  [CTH_TYPE__U64]           = "u64",

  [CTH_TYPE__I8]            = "i8",
  [CTH_TYPE__I16]           = "i16",
  [CTH_TYPE__I32]           = "i32",
  [CTH_TYPE__I64]           = "i64",

  [CTH_TYPE__F8]            = "f8",
  [CTH_TYPE__F16]           = "f16",
  [CTH_TYPE__F32]           = "f32",
  [CTH_TYPE__F64]           = "f64",

  [CTH_TYPE__CHAR]          = "char",
  [CTH_TYPE__SCHAR]         = "signed char",
  [CTH_TYPE__UCHAR]         = "unsigned char",
  [CTH_TYPE__SHORT]         = "short",
  [CTH_TYPE__USHORT]        = "unsigned short",
  [CTH_TYPE__INT]           = "int",
  [CTH_TYPE__UINT]          = "unsigned int",
  [CTH_TYPE__LONG]          = "long",
  [CTH_TYPE__ULONG]         = "unsigned long",
  [CTH_TYPE__LONG_LONG]     = "long long",
  [CTH_TYPE__ULONG_LONG]    = "unsigned long long",
  [CTH_TYPE__FLOAT]         = "float",
  [CTH_TYPE__DOUBLE]        = "double",
  [CTH_TYPE__LONG_DOUBLE]   = "long double",

  [CTH_TYPE__SSIZE]         = "ssize_t",
  [CTH_TYPE__SIZE]          = "size_t",

  [CTH_TYPE__SIGNED]        = "signed",
  [CTH_TYPE__UNSIGNED]      = "unsigned",

  [CTH_TYPE__BOOLEAN]       = "bool",
  [CTH_TYPE__TYPE]          = "type",
  [CTH_TYPE__QUALIFIER]     = "_Qualifier_t",               // for internal use only.
  [CTH_TYPE__NULLPTR]       = "nullptr_t",                  // idk why someone would use it, bring more in line with C's nullptr_t type l8r on.

  [CTH_TYPE__NONE]          = "ERROR",
};

// technically supports user-defined qualifiers, but those are an abstract idea anyways.
enum{
  CTH_QUALIFIER_MIN         = CTH_TYPE_MAX,

  CTH_QUALIFIER__CONST      = CTH_QUALIFIER_MIN,
  CTH_QUALIFIER__IMMUT      = CTH_QUALIFIER__CONST + 2,
  CTH_QUALIFIER__CONSTEXPR  = CTH_QUALIFIER__IMMUT + 2,

  CTH_QUALIFIER__PTR        = CTH_QUALIFIER__CONSTEXPR + 2,
  CTH_QUALIFIER__REF        = CTH_QUALIFIER__PTR + 2,

  CTH_QUALIFIER_MAX         = CTH_QUALIFIER__REF + 2,
};

static bool cth_qualifier__isdecl(const cth_nameT qualif){
  return qualif >= CTH_QUALIFIER__CONST && qualif <= CTH_QUALIFIER__CONSTEXPR;
}

// only predefined types are _Qualifier_t-typed.
const char* const restrict cth_qualifier__strings[] = {
  [CTH_QUALIFIER__CONST]            = "const",
  [CTH_QUALIFIER__IMMUT]            = "immut",
  [CTH_QUALIFIER__CONSTEXPR]        = "constexpr",
  [CTH_QUALIFIER__PTR]              = "*",
  [CTH_QUALIFIER__REF]              = "&"
};

static cth_type__internalT* cth_type__internal_init(){
  cth_type__internalT* const restrict t = malloc(sizeof(*t));
  
  t->tree = NULL;
  t->rc = 1;

  return t;
}

static cth_type__internalT* cth_type__internal_init__id(const cth_nameT id){
  cth_type__internalT* const restrict t = malloc(sizeof(*t));
// sets t->tree to this node.
  cth_ast_init(&t->tree, CTH_TOK__NAME, 0, 0, CTH_AST__ID);
  t->tree->data.id = id;

  t->rc = 1;

  return t;
}

static cth_type__internalT* cth_type__internal_clone(cth_type__internalT* const restrict src){
  cth_type__internalT* const restrict dst = cth_type__internal_init();

  dst->tree = cth_ast_clone(src->tree);

  return dst;
}

static cth_type__internalT* cth_type__internal_acquire(cth_type__internalT* const restrict t){
  t->rc++;
  return t;
}

static void cth_type__internal_destroy(cth_type__internalT* const restrict t){
  if(--t->rc) return;

  if(t->tree)
    cth_ast_destroy(t->tree);
  free(t);
}

// adds funcall to type ast.
static cth_astT* cth_type__ast_init(cth_variableT* const restrict qualified, const cth_nameT qualifier){
  cth_astT* const restrict type = qualified->internal.type->tree;
// sets qualified->internal.type->tree to funcall.
  cth_astT* const restrict funcall = cth_ast_init__weak(&qualified->internal.type->tree, CTH_TOK__FUNCALL, 0, 0, CTH_AST__FUNCALL);
// sets funcall->data._funcall.fun to this node.
  cth_astT* const restrict fun = cth_ast_init(&funcall->data._funcall.fun, CTH_TOK__NAME, 0, 0, CTH_AST__ID);
  fun->data.id = qualifier;
  funcall->data._funcall.args = vfastarr_init_explicit(1, 1, sizeof(*funcall->data._funcall.args));
  funcall->data._funcall.args[0] = type;
  type->reference = &funcall->data._funcall.args[0];

  return funcall;
}

#endif
