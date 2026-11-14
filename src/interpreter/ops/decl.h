#ifndef CTH_INTERPRETER_OPS_DECL_H_INCLUDED
#define CTH_INTERPRETER_OPS_DECL_H_INCLUDED

// declaration handling.

#include "../ctx.h"                     // ast, ctx stuff.
#include "typecheck.h"                  // typechecking ops.
#include "../../common/_common.h"       // variableT & more.

static cth_variableT* cth_op__declaration_type(const cth_astT* const restrict node, const size_t idx, cth_variable_ctxT* const restrict variables, rcT* const restrict path, cth_error_ctxT* const restrict err){
  cth_variableT* const type = cth_interpret(node->data._declaration.types[idx], variables, path, err);
  if(!type) return NULL;
  if(!cth_typecheck_top_lvl_is(CTH_TYPE__TYPE, type->type)){
    cth_error_report__type(CTH_ERROR__DECLARATION_INVALID_TYPE, CTH_ERROR__COMPILER, path, node->line, node->offset, err, variables, type->type);
    if(type->state & CTH_VARSTATE__FREEALL) cth_variable_destroy(type);
    return NULL;
  }
  return type;
}

static cth_variableT* cth_op__declaration_result(
  const cth_astT* const restrict node, const size_t idx, cth_variableT* const type,
  cth_variable_ctxT* const restrict variables, rcT* const restrict path, cth_error_ctxT* const restrict err
){
  cth_variableT* result = NULL;
  if(node->data._declaration.expr[idx]){
    result = cth_interpret(node->data._declaration.expr[idx], variables, path, err);

    if(!cth_typecheck_compatible(
      (const cth_variableT*[]){result->type, type}, 2,        // order of this array is important.
      CTH_TOK__ASSIGNMENT, NULL,
      variables
    ))
      cth_error_report__type_2(CTH_ERROR__DECLARATION_INCOMPATIBLE_TYPE, CTH_ERROR__COMPILER, path, node->line, node->offset, err, variables, type, result->type);
  }

  return result;
}

static cth_variableT* cth_op__declaration(const cth_astT* const restrict node, cth_variable_ctxT* const restrict variables, rcT* const restrict path, cth_error_ctxT* const restrict err){
  if(node->type != CTH_AST__DECLARATION){
    cth_error_report__u32_2(CTH_ERROR__INTERPRETER_AST_TYPE, CTH_ERROR__COMPILER, path, node->line, node->offset, err, node->type, CTH_AST__DECLARATION);
    return NULL;
  }

  const size_t c_decl = vfastarr_elcnt(node->data._declaration.vars, sizeof(*node->data._declaration.vars));
  for(size_t i = 0; i < c_decl; i++){
    cth_variableT* const type = cth_op__declaration_type(node, i, variables, path, err);
    if(!type) return NULL;
    cth_variableT* const result = cth_op__declaration_result(node, i, type, variables, path, err);
// guaranteed to exist.
    cth_variableT* const declared = cth_variable_search(variables, node->data._declaration.vars[i]);
    if(declared->type != cth_typevar(CTH_TYPE__NONE)){
      cth_error_report__str_size(CTH_ERROR__DECLARATION_REDEFINITION, CTH_ERROR__COMPILER, path, node->line, node->offset, err, declared->name, declared->s_name);
      if(type->state & CTH_VARSTATE__FREEALL) cth_variable_destroy(type);
      if(result && result->state & CTH_VARSTATE__FREEALL) cth_variable_destroy(result);
      return NULL;
    }

    declared->type = type;                    // whole reason for why I switched away from nameT to variableT in variableT.type field.
    if(node->data._declaration.expr[i]){
      cth_variable__internal_clone(declared, result);
      if(type->state & CTH_VARSTATE__FREEALL) cth_variable_destroy(type);
      if(result && result->state & CTH_VARSTATE__FREEALL) cth_variable_destroy(result);
    }
  }

  return NULL;
}

#endif
