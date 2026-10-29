#ifndef CTH_INTERPRETER_OPS_ASSIGNMENT_H_INCLUDED
#define CTH_INTERPRETER_OPS_ASSIGNMENT_H_INCLUDED

// arithmetic cth ops.

#include "../ctx.h"                     // ast, ctx stuff.
#include "../../common/_common.h"       // variableT & more.

static cth_variableT* cth_op__assignment(const cth_astT* const restrict node, cth_variable_ctxT* const restrict variables, rcT* const restrict path, cth_error_ctxT* const restrict err){
  if(node->type != CTH_AST__BINARY){
    cth_error_report__u32_2(CTH_ERROR__INTERPRETER_AST_TYPE, CTH_ERROR__COMPILER, path, node->line, node->offset, err, node->type, CTH_AST__BINARY);
    return NULL;
  }
  if(!node->data._binary.s0){
    cth_error_report__u32(CTH_ERROR__IMPOSSIBILITY, CTH_ERROR__COMPILER, path, node->line, node->offset, err, CTH_IMPOSSIBILITY__ASSIGNMENT_S0);
    return NULL;
  }
  if(node->data._binary.s0->type != CTH_AST__ID){
    cth_error_report(CTH_ERROR__ASSIGNMENT_RVALUE, CTH_ERROR__COMPILER, path, node->line, node->offset, err);
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

  const size_t s_internal = cth_type__sizeof(v0);
  v1->internal = malloc(s_internal);
  memcpy(v1->internal, v0->internal, s_internal);

  if(v0->qualifier & CTH_QUALIFIER__FREEALL) cth_variable_destroy(v0);

  return v1;
}

#endif
