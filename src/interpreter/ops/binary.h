#ifndef CTH_INTERPRETER_OPS_BINARY_H_INCLUDED
#define CTH_INTERPRETER_OPS_BINARY_H_INCLUDED

// arithmetic cth ops.

#include "../ctx.h"                     // ast, ctx stuff.
#include "../../common/_common.h"       // variableT & more.

static cth_variableT* cth_op__add(const cth_astT* const restrict node, cth_variable_ctxT* const restrict variables, rcT* const restrict path, cth_error_ctxT* const restrict err){
  if(node->type != CTH_AST__BINARY){
    cth_error_report__u32_2(CTH_ERROR__INTERPRETER_AST_TYPE, CTH_ERROR__COMPILER, path, node->line, node->offset, err, node->type, CTH_AST__BINARY);
    return NULL;
  }

  cth_variableT *const v0 = cth_interpret(node->data._binary.s0, variables, path, err),
                *const v1 = cth_interpret(node->data._binary.s0, variables, path, err);

  if(v0->type != v1->type){
    cth_error_report__type_2(CTH_ERROR__TYPE_INCOMPATIBLE_OP, CTH_ERROR__COMPILER, path, node->line, node->offset, err, v0->type, v1->type);
// TODO: return error valu.
    return NULL;
  }

  void* const raw = malloc(cth_type__sizeof(v0));
  switch(v0->type){
    case CTH_TYPE__I8:  
    case CTH_TYPE__I16:
    case CTH_TYPE__I32: *(int32_t*)raw = *(int32_t*)v0->internal + *(int32_t*)v1->internal; break;
    case CTH_TYPE__I64:

    case CTH_TYPE__U8:
    case CTH_TYPE__U16:
    case CTH_TYPE__U32:
    case CTH_TYPE__U64:

    case CTH_TYPE__F8:
    case CTH_TYPE__F16:
    case CTH_TYPE__F32:
    case CTH_TYPE__F64:

    case CTH_TYPE__CHAR:
    case CTH_TYPE__BOOLEAN:
  }

  cth_variableT* const restrict result = cth_variable_init(NULL, 0, raw, v0->type, cth_qualifier_op2(v0->qualifier, v1->qualifier) | CTH_QUALIFIER__FREEALL);

  if(v0->qualifier & CTH_QUALIFIER__FREEALL) cth_variable_destroy(v0);
  if(v1->qualifier & CTH_QUALIFIER__FREEALL) cth_variable_destroy(v1);

  return result;
}

#endif
