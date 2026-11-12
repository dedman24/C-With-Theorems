#ifndef CTH_INTERPRETER_OPS_MISC_H_INCLUDED
#define CTH_INTERPRETER_OPS_MISC_H_INCLUDED

#include "../ctx.h"                     // ast, ctx stuff.
#include "../../common/_common.h"       // variableT & more.

static cth_variableT* cth_op__name(const cth_astT* const restrict node, cth_variable_ctxT* const restrict variables, rcT* const restrict path, cth_error_ctxT* const restrict err){
  if(node->type != CTH_AST__ID){
    cth_error_report__u32_2(CTH_ERROR__INTERPRETER_AST_TYPE, CTH_ERROR__COMPILER, path, node->line, node->offset, err, node->type, CTH_AST__ID);
    return NULL;
  }

  return cth_variable_search(variables, node->data.id);
}

static cth_variableT* cth_op__comma(const cth_astT* const restrict node, cth_variable_ctxT* const restrict variables, rcT* const restrict path, cth_error_ctxT* const restrict err){
  if(node->type != CTH_AST__BINARY){
    cth_error_report__u32_2(CTH_ERROR__INTERPRETER_AST_TYPE, CTH_ERROR__COMPILER, path, node->line, node->offset, err, node->type, CTH_AST__BINARY);
    return NULL;
  }
  cth_variableT *const v0 = cth_interpret(node->data._binary.s0, variables, path, err),
                *const v1 = cth_interpret(node->data._binary.s1, variables, path, err);
  if(!v0 || !v1) return NULL;
  if(v1->state & CTH_VARSTATE__FREEALL) cth_variable_destroy(v1);

  return v0;
}

#endif
