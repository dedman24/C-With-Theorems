#ifndef CTH_INTERPRETER_OPS_IF_H_INCLUDED
#define CTH_INTERPRETER_OPS_IF_H_INCLUDED

// if statement handling (both normal and ternary if compile to the same token).

#include "../ctx.h"
#include "typecheck.h"

static cth_variableT* cth_op__if(const cth_astT* const restrict node, cth_variable_ctxT* const restrict variables, rcT* const restrict path, cth_error_ctxT* const restrict err){
  if(node->type != CTH_AST__IF){
    cth_error_report__u32_2(CTH_ERROR__INTERPRETER_AST_TYPE, CTH_ERROR__COMPILER, path, node->line, node->offset, err, node->type, CTH_AST__BINARY);
    return NULL;
  }

  cth_variableT* const condition = cth_interpret(node->data._if.condition, variables, path, err);
  if(!cth_typecheck_top_lvl_is(CTH_TYPE__BOOLEAN, condition->type)){
    cth_error_report__type_2(CTH_ERROR__TYPE_INCOMPATIBLE_OP, CTH_ERROR__COMPILER, path, node->line, node->offset, err, variables, condition->type, cth_typevar(CTH_TYPE__BOOLEAN));
    return NULL;
  }
  cth_astT** const program = condition->internal.boolean? node->data._if.success: node->data._if.failure;

  const size_t c_program = vfastarr_elcnt(program, sizeof(*program));
  cth_variableT* eval;
  for(size_t i = 0; i < c_program; i++){    
    cth_variableT* const result = cth_interpret(program[i], variables, path, err);
    if(i == c_program - 1)
      eval = result;
  }
  return eval;
}

#endif
