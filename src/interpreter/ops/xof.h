#ifndef CTH_INTERPRETER_OPS_XOF_H_INCLUDED
#define CTH_INTERPRETER_OPS_XOF_H_INCLUDED

// typeof, sizeof, countof unary operators.

#include "../ctx.h"

static cth_variableT* cth_op__typeof(const cth_astT* const restrict node, cth_variable_ctxT* const restrict variables, rcT* const restrict path, cth_error_ctxT* const restrict err){
  if(node->type != CTH_AST__UNARY){
    cth_error_report__u32_2(CTH_ERROR__INTERPRETER_AST_TYPE, CTH_ERROR__COMPILER, path, node->line, node->offset, err, node->type, CTH_AST__UNARY);
    return NULL;
  }

  cth_variableT* const src = cth_interpret(node->data._unary.src, variables, path, err);
  if(!src) return NULL;

  cth_variableT* const type = src->type;
  if(src->state & CTH_VARSTATE__FREEALL) cth_variable_destroy(src);

  return type;
}

#endif
