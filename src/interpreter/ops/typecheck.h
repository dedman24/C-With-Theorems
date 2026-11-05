#ifndef CTH_INTERPRETER_OPS_TYPECHECK_H_INCLUDED
#define CTH_INTERPRETER_OPS_TYPECHECK_H_INCLUDED

#include "../ctx.h"                 // cth_astT.
#include "../../common/_common.h"   // variableT, errors.

// a top-level qualifier is...
// I have an idea, IDK how to describe it.
// basically all above *, [] & similar.

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
  cth_variableT* const vtype = cth_variable_search(variables, v1->type);

  if(v0->type != CTH_TYPE__TYPE)
    cth_error_report(CTH_ERROR__TYPEIS_SECOND_ARG, CTH_ERROR__COMPILER, path, node->line, node->offset, err);
  else if(vtype != v0)
    cth_error_report__str_2(CTH_ERROR__TYPE_INCOMPATIBLE_OP, CTH_ERROR__COMPILER, path, node->line, node->offset, err, vtype->name, v1->name);

  if(v0->state & CTH_VARSTATE__FREEALL) cth_variable_destroy(v0);
  if(v1->state & CTH_VARSTATE__FREEALL) cth_variable_destroy(v1);

  return NULL;
}

// returns first qualif that is in top lvl, 0 otherwise.
static cth_nameT cth_typecheck_top_lvl(
  cth_variableT* const restrict var, const cth_nameT qualif,
  rcT* const restrict path, const cth_fposT line, const cth_fposT offset, cth_error_ctxT* const restrict err
){
  cth_astT* node = var->internal.type->tree;

  while(1){
    switch(node->type){
      case CTH_AST__ID:
        return 0;
      case CTH_AST__FUNCALL:
        cth_astT* const node_qualif = node->data._funcall.fun;
        if(node_qualif->type != CTH_AST__ID)
          cth_error_report__u32(CTH_ERROR__IMPOSSIBILITY, CTH_ERROR__COMPILER, path, line, offset, err, CTH_IMPOSSIBILITY__TYPECHECK_TOP_LVL);
        if(node_qualif->data.id == qualif)
          return qualif;
        node = node->data._funcall.args[0];
        break;
      default:
        if(node_qualif->type != CTH_AST__ID)
          cth_error_report__u32(CTH_ERROR__IMPOSSIBILITY, CTH_ERROR__COMPILER, path, line, offset, err, CTH_IMPOSSIBILITY__TYPECHECK_TOP_LVL2);
        break;
    }
  }
}

// returns first qualif that is in top lvl, 0 otherwise.
static cth_nameT cth_typecheck_top_lvl__many(
  cth_variableT* const restrict var, const cth_nameT qualif[], const size_t c_qualif,
  rcT* const restrict path, const cth_fposT line, const cth_fposT offset, cth_error_ctxT* const restrict err
){
  cth_astT* node = var->internal.type->tree;

  while(1){
    switch(node->type){
      case CTH_AST__ID:
        return 0;
      case CTH_AST__FUNCALL:
        cth_astT* const node_qualif = node->data._funcall.fun;
        if(node_qualif->type != CTH_AST__ID)
          cth_error_report__u32(CTH_ERROR__IMPOSSIBILITY, CTH_ERROR__COMPILER, path, line, offset, err, CTH_IMPOSSIBILITY__TYPECHECK_TOP_LVL);
        for(size_t i = 0; i < c_qualif; i++){
          if(node_qualif->data.id == qualif[i])
            return qualif[i];
        }
        node = node->data._funcall.args[0];
        break;
      default:
        if(node_qualif->type != CTH_AST__ID)
          cth_error_report__u32(CTH_ERROR__IMPOSSIBILITY, CTH_ERROR__COMPILER, path, line, offset, err, CTH_IMPOSSIBILITY__TYPECHECK_TOP_LVL2);
        break;
    }
  }
}

struct cth_typecheck_top_lvl_getS{
  cth_astT* ast;                    // abstract syntax tree underlying `dispatch`.
  cth_nameT dispatch;               // type to dispatch compatibility on.
};

// gets top-level type of type `type`.
static struct cth_typecheck_top_lvl_getS cth_typecheck_top_lvl_get(cth_variableT* const restrict type){
  cth_astT* ast_type = type->internal.type->tree;

  while(1){
    switch(ast_type->type){
      case CTH_AST__ID:
        return (struct cth_typecheck_top_lvl_getS){ .ast = ast_type, .dispatch = ast_type->data.id };
      case CTH_AST__FUNCALL:
        cth_astT* fun = ast_type->data._funcall.fun;
        switch(fun->data.id){
          // case CTH_QUALIFIER__PTR:
          // case CTH_QUALIFIER__REF:
          //  return (struct cth_typecheck_top_lvl_getS){ .ast = ast_type->data._funcall.args[0], .dispatch = fun->data.id };
          default: break;
        }
        ast_type = ast_type->data._funcall.args[0];
        break;
      default:
        *(volatile char*)NULL = 67;
        break;
    }
  }
}

static bool cth_typecheck_compatible(const cth_nameT types[], const size_t c_types, cth_nameT* const restrict dispatch, cth_variable_ctxT* const restrict variables){
  if(c_types == 0) return false;              // 0 types is invalid, possibly an error.
  cth_variableT* const t0 = cth_variable_search(variables, types[0]);
  if(!t0 || t0->type != CTH_TYPE__TYPE) return false;
  const struct cth_typecheck_top_lvl_getS t0_top = cth_typecheck_top_lvl_get(t0);

  if(dispatch)
    *dispatch = t0_top.dispatch;

  for(size_t i = 1; i < c_types; i++){
    cth_variableT* const t = cth_variable_search(variables, types[i]);
    if(!t || t->type != CTH_TYPE__TYPE) return false;

    const struct cth_typecheck_top_lvl_getS top = cth_typecheck_top_lvl_get(t);
    switch(top.dispatch){
    //  case CTH_QUALIFIER__PTR:
    //  case CTH_QUALIFIER__REF:
    //    idk some ptr-specific function.
      default:                            // all other types compare by value.
        if(top.dispatch != t0_top.dispatch) return false;
    }
  }

  return true;
}

#endif
