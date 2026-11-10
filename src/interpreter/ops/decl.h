#ifndef CTH_INTERPRETER_OPS_DECL_H_INCLUDED
#define CTH_INTERPRETER_OPS_DECL_H_INCLUDED

// declaration handling.

#include "../ctx.h"                     // ast, ctx stuff.
#include "typecheck.h"                  // typechecking ops.
#include "../../common/_common.h"       // variableT & more.

static cth_variableT* cth_op__declaration(const cth_astT* const restrict node, cth_variable_ctxT* const restrict variables, rcT* const restrict path, cth_error_ctxT* const restrict err){
  if(node->type != CTH_AST__BINARY){
    cth_error_report__u32_2(CTH_ERROR__INTERPRETER_AST_TYPE, CTH_ERROR__COMPILER, path, node->line, node->offset, err, node->type, CTH_AST__BINARY);
    return NULL;
  }

  cth_variableT* const type = cth_interpret(node->data._declaration.type, variables, path, err);
  if(!type) return NULL;
  if(type->type != CTH_TYPE__TYPE){
    cth_error_report__type(CTH_ERROR__DECLARATION_INVALID_TYPE, CTH_ERROR__COMPILER, path, node->line, node->offset, err, variables, type->type);
    return NULL;
  }

  const size_t c_decl = vfastarr_elcnt(node->data._declaration.vars, sizeof(*node->data._declaration.vars));
  for(size_t i = 0; i < c_decl; i++){
    cth_variableT *result, *t_result;
    if(node->data._declaration.expr[i]){
      result = cth_interpret(node->data._declaration.expr[i], variables, path, err);
      t_result = cth_variable_search(variables, result->type);

      if(!cth_typecheck_compatible__vars((cth_variableT*[]){type, t_result}, 2, CTH_TOK__ASSIGNMENT, NULL, variables)){
        cth_error_report__typevar_2(CTH_ERROR__DECLARATION_INCOMPATIBLE_TYPE, CTH_ERROR__COMPILER, path, node->line, node->offset, err, variables, type, t_result);
        return NULL;
      }
    }

    cth_variableT* const declared = cth_variable_search(variables, node->data._declaration.vars[i]);
    if(declared->type != CTH_TYPE__NONE){
      cth_error_report__str_size(CTH_ERROR__DECLARATION_REDEFINITION, CTH_ERROR__COMPILER, path, node->line, node->offset, err, declared->name, declared->s_name);
      return NULL;
    }

    declared->type = 0;             // TODO: make this work!
    if(node->data._declaration.expr[i]){
      cth_variable__internal_clone(declared, result);
      if(result->state & CTH_VARSTATE__FREEALL) cth_variable_destroy(result);
    }
  }

  if(type->state & CTH_VARSTATE__FREEALL) cth_variable_destroy(type);
  return type;
}

#endif
