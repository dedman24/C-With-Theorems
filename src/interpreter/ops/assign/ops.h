#ifndef CTH_INTERPRETER_OPS_ASSIGN_OP_H_INCLUDED
#define CTH_INTERPRETER_OPS_ASSIGN_OP_H_INCLUDED

#include "../../ctx.h"          // basic ctx handling stuff.
#include "../binary.h"          // cth_op__add.
#include "assignment.h"         // assignment.

static cth_variableT* cth_op__assign_mul(const cth_astT* const restrict node, cth_variable_ctxT* const restrict variables, rcT* const restrict path, cth_error_ctxT* const restrict err){
  return NULL;
}

static cth_variableT* cth_op__assign_div(const cth_astT* const restrict node, cth_variable_ctxT* const restrict variables, rcT* const restrict path, cth_error_ctxT* const restrict err){
  return NULL;
}

static cth_variableT* cth_op__assign_mod(const cth_astT* const restrict node, cth_variable_ctxT* const restrict variables, rcT* const restrict path, cth_error_ctxT* const restrict err){
  return NULL;
}

static cth_variableT* cth_op__assign_add(const cth_astT* const restrict node, cth_variable_ctxT* const restrict variables, rcT* const restrict path, cth_error_ctxT* const restrict err){
  if(node->type != CTH_AST__BINARY){
    cth_error_report__u32_2(CTH_ERROR__INTERPRETER_AST_TYPE, CTH_ERROR__COMPILER, path, node->line, node->offset, err, node->type, CTH_AST__BINARY);
    return NULL;
  }
  if(node->data._binary.s1->type != CTH_AST__ID){
    cth_error_report(CTH_ERROR__ASSIGNMENT_RVALUE, CTH_ERROR__COMPILER, path, node->line, node->offset, err);
    return NULL;
  }

  cth_variableT *const v0 = cth_interpret(node->data._binary.s0, variables, path, err),
                *const v1 = cth_interpret(node->data._binary.s1, variables, path, err);

  cth_variableT* const restrict result = cth_op__add__internal(v0, v1, node, variables, path, err);
  if(!result){
    if(v0->state & CTH_VARSTATE__FREEALL) cth_variable_destroy(v0);
    if(v1->state & CTH_VARSTATE__FREEALL) cth_variable_destroy(v1);
    return NULL;
  }
  if(!cth_op__assignment__internal(result, v1, node, variables, path, err)){
    if(v0->state & CTH_VARSTATE__FREEALL) cth_variable_destroy(v0);
    if(v1->state & CTH_VARSTATE__FREEALL) cth_variable_destroy(v1);
    if(result->state & CTH_VARSTATE__FREEALL) cth_variable_destroy(result);
    return NULL;
  };
  if(v0->state & CTH_VARSTATE__FREEALL) cth_variable_destroy(v0);
  if(result->state & CTH_VARSTATE__FREEALL) cth_variable_destroy(result);
  return v1;
}

static cth_variableT* cth_op__assign_sub(const cth_astT* const restrict node, cth_variable_ctxT* const restrict variables, rcT* const restrict path, cth_error_ctxT* const restrict err){
  return NULL;
}

static cth_variableT* cth_op__assign_and(const cth_astT* const restrict node, cth_variable_ctxT* const restrict variables, rcT* const restrict path, cth_error_ctxT* const restrict err){
  return NULL;
}

static cth_variableT* cth_op__assign_or (const cth_astT* const restrict node, cth_variable_ctxT* const restrict variables, rcT* const restrict path, cth_error_ctxT* const restrict err){
  return NULL;
}

static cth_variableT* cth_op__assign_xor(const cth_astT* const restrict node, cth_variable_ctxT* const restrict variables, rcT* const restrict path, cth_error_ctxT* const restrict err){
  return NULL;
}

static cth_variableT* cth_op__assign_shl(const cth_astT* const restrict node, cth_variable_ctxT* const restrict variables, rcT* const restrict path, cth_error_ctxT* const restrict err){
  return NULL;
}

static cth_variableT* cth_op__assign_shr(const cth_astT* const restrict node, cth_variable_ctxT* const restrict variables, rcT* const restrict path, cth_error_ctxT* const restrict err){
  return NULL;
}

static cth_variableT* cth_op__assign_rol(const cth_astT* const restrict node, cth_variable_ctxT* const restrict variables, rcT* const restrict path, cth_error_ctxT* const restrict err){
  return NULL;
}

static cth_variableT* cth_op__assign_ror(const cth_astT* const restrict node, cth_variable_ctxT* const restrict variables, rcT* const restrict path, cth_error_ctxT* const restrict err){
  return NULL;
}


#endif
