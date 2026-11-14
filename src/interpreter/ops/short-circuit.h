#ifndef CTH_INTERPRETER_OPS_SHORT_CIRCUIT_H_INCLUDED
#define CTH_INTERPRETER_OPS_SHORT_CIRCUIT_H_INCLUDED

// short-circuiting `&&`, `||`, `^^` ops.

#include "../ctx.h"                     // ast, ctx stuff.
#include "typecheck.h"                  // typechecking ops.


static cth_variableT* cth_op__andl(const cth_astT* const restrict node, cth_variable_ctxT* const restrict variables, rcT* const restrict path, cth_error_ctxT* const restrict err){
  if(node->type != CTH_AST__BINARY){
    cth_error_report__u32_2(CTH_ERROR__INTERPRETER_AST_TYPE, CTH_ERROR__COMPILER, path, node->line, node->offset, err, node->type, CTH_AST__BINARY);
    return NULL;
  }

  {
    cth_variableT *const v0 = cth_interpret(node->data._binary.s0, variables, path, err);
    if(!v0) return NULL;
    if(!cth_typecheck_top_lvl_is(CTH_TYPE__BOOLEAN, v0->type)){
      cth_error_report__type_2(CTH_ERROR__TYPE_INCOMPATIBLE_OP, CTH_ERROR__COMPILER, path, node->line, node->offset, err, variables, v0->type, cth_typevar(CTH_TYPE__BOOLEAN));
      return NULL;
    }
    const bool truthy = v0->internal.boolean;
    if(v0->state & CTH_VARSTATE__FREEALL) cth_variable_destroy(v0);
    if(!truthy) return cth_variable__false;
  }
  {
    cth_variableT *const v1 = cth_interpret(node->data._binary.s1, variables, path, err);
    if(!v1) return NULL;
    if(!cth_typecheck_top_lvl_is(CTH_TYPE__BOOLEAN, v1->type)){
      cth_error_report__type_2(CTH_ERROR__TYPE_INCOMPATIBLE_OP, CTH_ERROR__COMPILER, path, node->line, node->offset, err, variables, v1->type, cth_typevar(CTH_TYPE__BOOLEAN));
      return NULL;
    }
    const bool truthy = v1->internal.boolean;
    if(v1->state & CTH_VARSTATE__FREEALL) cth_variable_destroy(v1);
    if(!truthy) return cth_variable__false;
  }

  return cth_variable__true;
}

static cth_variableT* cth_op__orl(const cth_astT* const restrict node, cth_variable_ctxT* const restrict variables, rcT* const restrict path, cth_error_ctxT* const restrict err){
  if(node->type != CTH_AST__BINARY){
    cth_error_report__u32_2(CTH_ERROR__INTERPRETER_AST_TYPE, CTH_ERROR__COMPILER, path, node->line, node->offset, err, node->type, CTH_AST__BINARY);
    return NULL;
  }

  {
    cth_variableT *const v0 = cth_interpret(node->data._binary.s0, variables, path, err);
    if(!v0) return NULL;
    if(!cth_typecheck_top_lvl_is(CTH_TYPE__BOOLEAN, v0->type)){
      cth_error_report__type_2(CTH_ERROR__TYPE_INCOMPATIBLE_OP, CTH_ERROR__COMPILER, path, node->line, node->offset, err, variables, v0->type, cth_typevar(CTH_TYPE__BOOLEAN));
      return NULL;
    }
    const bool truthy = v0->internal.boolean;
    if(v0->state & CTH_VARSTATE__FREEALL) cth_variable_destroy(v0);
    if(truthy) return cth_variable__true;
  }
  {
    cth_variableT *const v1 = cth_interpret(node->data._binary.s1, variables, path, err);
    if(!v1) return NULL;
    if(!cth_typecheck_top_lvl_is(CTH_TYPE__BOOLEAN, v1->type)){
      cth_error_report__type_2(CTH_ERROR__TYPE_INCOMPATIBLE_OP, CTH_ERROR__COMPILER, path, node->line, node->offset, err, variables, v1->type, cth_typevar(CTH_TYPE__BOOLEAN));
      return NULL;
    }
    const bool truthy = v1->internal.boolean;
    if(v1->state & CTH_VARSTATE__FREEALL) cth_variable_destroy(v1);
    if(truthy) return cth_variable__true;
  }

  return cth_variable__false;
}

// xorl cannot perform short-circuiting.
static cth_variableT* cth_op__xorl(const cth_astT* const restrict node, cth_variable_ctxT* const restrict variables, rcT* const restrict path, cth_error_ctxT* const restrict err){
  if(node->type != CTH_AST__BINARY){
    cth_error_report__u32_2(CTH_ERROR__INTERPRETER_AST_TYPE, CTH_ERROR__COMPILER, path, node->line, node->offset, err, node->type, CTH_AST__BINARY);
    return NULL;
  }

  cth_variableT *const v0 = cth_interpret(node->data._binary.s0, variables, path, err),
                *const v1 = cth_interpret(node->data._binary.s1, variables, path, err);
  if(!v0 || !v1){
    if(v0 && v0->state & CTH_VARSTATE__FREEALL) cth_variable_destroy(v0);
    if(v1 && v1->state & CTH_VARSTATE__FREEALL) cth_variable_destroy(v1);
    return NULL;
  }
  if(!cth_typecheck_top_lvl_is(CTH_TYPE__BOOLEAN, v0->type)){
    cth_error_report__type_2(CTH_ERROR__TYPE_INCOMPATIBLE_OP, CTH_ERROR__COMPILER, path, node->line, node->offset, err, variables, v0->type, cth_typevar(CTH_TYPE__BOOLEAN));
    return NULL;
  }
  if(!cth_typecheck_top_lvl_is(CTH_TYPE__BOOLEAN, v1->type)){
    cth_error_report__type_2(CTH_ERROR__TYPE_INCOMPATIBLE_OP, CTH_ERROR__COMPILER, path, node->line, node->offset, err, variables, v1->type, cth_typevar(CTH_TYPE__BOOLEAN));
    return NULL;
  }
  const bool truthy = v0->internal.boolean ^ v1->internal.boolean;
  if(v0->state & CTH_VARSTATE__FREEALL) cth_variable_destroy(v0);
  if(v1->state & CTH_VARSTATE__FREEALL) cth_variable_destroy(v1);

  return truthy? cth_variable__true: cth_variable__false;
}


#endif
