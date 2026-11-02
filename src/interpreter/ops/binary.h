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
                *const v1 = cth_interpret(node->data._binary.s1, variables, path, err);
  if(!v0 || !v1){
    cth_error_report__u32(CTH_ERROR__IMPOSSIBILITY, CTH_ERROR__COMPILER, path, node->line, node->offset, err, CTH_IMPOSSIBILITY__MISSING_SRC);
    return NULL;
  }

  if(v0->type != v1->type){
    cth_error_report__type_2(CTH_ERROR__TYPE_INCOMPATIBLE_OP, CTH_ERROR__COMPILER, path, node->line, node->offset, err, v0->type, v1->type);
    return NULL;
  }

  cth_variableT* const restrict result = cth_variable_init(NULL, 0, v0->type, cth_qualifier_op2(v0->state, v1->state) | CTH_VARSTATE__FREEALL);  
  switch(v0->type){
    case CTH_TYPE__I8:  
    case CTH_TYPE__I16:
    case CTH_TYPE__I32: result->internal.i32 = v0->internal.i32 + v1->internal.i32; break;
    case CTH_TYPE__I64: result->internal.i64 = v0->internal.i64 + v1->internal.i64; break;

    case CTH_TYPE__U8:
    case CTH_TYPE__U16:
    case CTH_TYPE__U32:
    case CTH_TYPE__U64:

    case CTH_TYPE__F8:
    case CTH_TYPE__F16:
    case CTH_TYPE__F32:
    case CTH_TYPE__F64:

    case CTH_TYPE__CHAR8:
    case CTH_TYPE__BOOLEAN:
  }

  if(v0->state & CTH_VARSTATE__FREEALL) cth_variable_destroy(v0);
  if(v1->state & CTH_VARSTATE__FREEALL) cth_variable_destroy(v1);

  return result;
}

#endif
