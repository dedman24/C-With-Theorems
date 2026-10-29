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

// performs cth type checking.
static cth_variableT* cth_op__typechecking(const cth_astT* const restrict node, cth_variable_ctxT* const restrict variables, rcT* const restrict path, cth_error_ctxT* const restrict err){
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
  cth_variableT* const vtype = cth_variable_search(variables, v0->type);

  if(v1->type != CTH_TYPE__TYPE)
    cth_error_report(CTH_ERROR__TYPEIS_SECOND_ARG, CTH_ERROR__COMPILER, path, node->line, node->offset, err);
  else if(vtype != v1)
    cth_error_report__str_2(CTH_ERROR__TYPE_INCOMPATIBLE_OP, CTH_ERROR__COMPILER, path, node->line, node->offset, err, vtype->name, v1->name);

  if(v0->qualifier & CTH_QUALIFIER__FREEALL) cth_variable_destroy(v0);
  if(v1->qualifier & CTH_QUALIFIER__FREEALL) cth_variable_destroy(v1);

  return NULL;
}

static cth_variableT* cth_op__comma(const cth_astT* const restrict node, cth_variable_ctxT* const restrict variables, rcT* const restrict path, cth_error_ctxT* const restrict err){
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
  cth_variableT* const vtype = cth_variable_search(variables, v0->type);

  if(v1->type != CTH_TYPE__TYPE)
    cth_error_report(CTH_ERROR__TYPEIS_SECOND_ARG, CTH_ERROR__COMPILER, path, node->line, node->offset, err);
  else if(vtype != v1)
    cth_error_report__str_2(CTH_ERROR__TYPE_INCOMPATIBLE_OP, CTH_ERROR__COMPILER, path, node->line, node->offset, err, vtype->name, v1->name);

  if(v1->qualifier & CTH_QUALIFIER__FREEALL) cth_variable_destroy(v1);

  return v0;
}

#endif
