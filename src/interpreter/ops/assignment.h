#ifndef CTH_INTERPRETER_OPS_ASSIGNMENT_H_INCLUDED
#define CTH_INTERPRETER_OPS_ASSIGNMENT_H_INCLUDED

// arithmetic cth ops.

#include "../ctx.h"                     // ast, ctx stuff.
#include "typecheck.h"                  // typechecking ops.
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
  if(node->data._binary.s1->type != CTH_AST__ID){
    cth_error_report(CTH_ERROR__ASSIGNMENT_RVALUE, CTH_ERROR__COMPILER, path, node->line, node->offset, err);
    return NULL;
  }

  cth_variableT *const v0 = cth_interpret(node->data._binary.s0, variables, path, err),
                *const v1 = cth_interpret(node->data._binary.s1, variables, path, err);
  if(!v0 || !v1){
    cth_error_report__u32(CTH_ERROR__IMPOSSIBILITY, CTH_ERROR__COMPILER, path, node->line, node->offset, err, CTH_IMPOSSIBILITY__MISSING_SRC);
    return NULL;
  }
  if(v1->state & CTH_VARSTATE__IMMEDIATE){
    cth_error_report(CTH_ERROR__ASSIGNMENT_IMMEDIATE, CTH_ERROR__COMPILER, path, node->line, node->offset, err);
    return NULL;
  }

  if(!cth_typecheck_compatible((cth_nameT[]){v0->type, v1->type}, 2, node->op, NULL, variables)){
    cth_error_report__type_2(CTH_ERROR__TYPE_INCOMPATIBLE_OP, CTH_ERROR__COMPILER, path, node->line, node->offset, err, variables, v0->type, v1->type);
    return NULL;
  }
// v1 ~ assigned.
// v0 ~ assignee.
  cth_variableT* const type_v1 = cth_variable_search(variables, v1->type);
  const cth_nameT conflicting = cth_typecheck_top_lvl__many(
    type_v1,
    (cth_nameT[]){ CTH_QUALIFIER__CONST, CTH_QUALIFIER__IMMUT, CTH_QUALIFIER__CONSTEXPR }, 3,
    path, node->line, node->offset, err
  );
  if(conflicting){
    if(v1->state & CTH_VARSTATE__ASSIGNED){
      cth_error_report__qualif(CTH_ERROR__ASSIGNMENT_CONST, CTH_ERROR__COMPILER, path, node->line, node->offset, err, conflicting);
      return NULL;
    }
    else v1->state |= CTH_VARSTATE__ASSIGNED;
  }

  cth_variable__internal_clone(v1, v0);

  if(v0->state & CTH_VARSTATE__FREEALL) cth_variable_destroy(v0);

  return v1;
}

#endif
