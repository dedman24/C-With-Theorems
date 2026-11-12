#ifndef CTH_INTERPRETER_OPS_TYPECHECK_H_INCLUDED
#define CTH_INTERPRETER_OPS_TYPECHECK_H_INCLUDED

#include "../ctx.h"                 // cth_astT.
#include "../../common/_common.h"   // variableT, errors.

// this file concerns all that type-checking must consider in cth.

// a top-level qualifier is...
// I have an idea, IDK how to describe it.
// basically all above *, [] & similar.

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
static struct cth_typecheck_top_lvl_getS cth_typecheck_top_lvl_get(cth_astT* ast_type){
  while(1){
    switch(ast_type->type){
      case CTH_AST__ID:
        return (struct cth_typecheck_top_lvl_getS){ .ast = ast_type, .dispatch = ast_type->data.id };
      case CTH_AST__FUNCALL:
        cth_astT* fun = ast_type->data._funcall.fun;
        switch(fun->data.id){
           case CTH_QUALIFIER__PTR:
           case CTH_QUALIFIER__REF:
            return (struct cth_typecheck_top_lvl_getS){ .ast = ast_type->data._funcall.args[0], .dispatch = fun->data.id };
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

static bool cth_typecheck_top_lvl_is(const cth_nameT type, const cth_variableT* const var_type){
  cth_astT* ast_type = var_type->internal.type->tree;

  while(1){
    switch(ast_type->type){
      case CTH_AST__ID:
        return ast_type->data.id == type;
      case CTH_AST__FUNCALL:
        cth_astT* fun = ast_type->data._funcall.fun;
        switch(fun->data.id){
           case CTH_QUALIFIER__PTR:
           case CTH_QUALIFIER__REF:
            return fun->data.id == type;
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

static bool cth_typecheck_compatible__ptr(
  const cth_nameT dispatch0, cth_astT* const ast0, const cth_nameT dispatch1, cth_astT* const ast1, const cth_tokT op
){
  switch(op){
    case CTH_TOK__PLUS:
      return cth_type__isint(dispatch1);
    case CTH_TOK__ASSIGNMENT:
    // NULL support.
      if(dispatch0 == CTH_TYPE__NULLPTR)
        return true;
      if(dispatch1 != dispatch0)
        return false;
      const struct cth_typecheck_top_lvl_getS top0 = cth_typecheck_top_lvl_get(ast0), top1 =  cth_typecheck_top_lvl_get(ast1);
      return top0.dispatch == top1.dispatch;
    default:
      return false;
  }
}

// op ~ operation under which typechecking is being performed.
//   important because, for example, `+` is compatible only on ptr-integer, but assignment is compatible only on ptr-ptr.
//   certain ops are never valid on pointers.
static bool cth_typecheck_compatible(
  const cth_variableT* const types[], const size_t c_types, const cth_tokT op,
  cth_nameT* const restrict dispatch, cth_variable_ctxT* const restrict variables
){
  if(c_types == 0) return false;              // 0 types is invalid, possibly an error.
  if(!types[0] || !cth_typecheck_top_lvl_is(CTH_TYPE__TYPE, types[0]->type)) return false;
  const struct cth_typecheck_top_lvl_getS t0_top = cth_typecheck_top_lvl_get(types[0]->internal.type->tree);

  if(dispatch)
    *dispatch = t0_top.dispatch;

  for(size_t i = 1; i < c_types; i++){
    if(!types[i] || !cth_typecheck_top_lvl_is(CTH_TYPE__TYPE, types[i]->type)) return false;

    const struct cth_typecheck_top_lvl_getS top = cth_typecheck_top_lvl_get(types[i]->internal.type->tree);
    switch(top.dispatch){
      case CTH_TYPE__NULLPTR:
      case CTH_QUALIFIER__PTR:
      case CTH_QUALIFIER__REF:
        return cth_typecheck_compatible__ptr(t0_top.dispatch, t0_top.ast, top.dispatch, top.ast, op);
      default:                            // all other types compare by value.
        if(top.dispatch != t0_top.dispatch) return false;
    }
  }

  return true;
}

// two types are equal when their contents are.
static bool cth_typecheck_equal(const cth_variableT* const t0, const cth_variableT* const t1){
// simple optimisation by checking that pointers are equal.
// if they are, they _must_ be pointing to the same thing.
  if(t0 == t1) return true;
  if(t0->internal.type == t1->internal.type) return true;
  return cth_ast_equal(t0->internal.type->tree, t1->internal.type->tree);
}

// performs cth type checking.
static cth_variableT* cth_op__typechecking(const cth_astT* const restrict node, cth_variable_ctxT* const restrict variables, rcT* const restrict path, cth_error_ctxT* const restrict err){
  if(node->type != CTH_AST__BINARY){
    cth_error_report__u32_2(CTH_ERROR__INTERPRETER_AST_TYPE, CTH_ERROR__COMPILER, path, node->line, node->offset, err, node->type, CTH_AST__BINARY);
    return NULL;
  }
  cth_variableT *const v0 = cth_interpret(node->data._binary.s0, variables, path, err),
                *const v1 = cth_interpret(node->data._binary.s1, variables, path, err);
  if(!v0 || !v1) return NULL;

  if(!cth_typecheck_top_lvl_is(CTH_TYPE__TYPE, v0->type))
    cth_error_report(CTH_ERROR__TYPEIS_SECOND_ARG, CTH_ERROR__COMPILER, path, node->line, node->offset, err);
  else if(v0->type != v1)
    cth_error_report__str_2(CTH_ERROR__TYPE_INCOMPATIBLE_OP, CTH_ERROR__COMPILER, path, node->line, node->offset, err, v0->name, v1->name);

  if(v0->state & CTH_VARSTATE__FREEALL) cth_variable_destroy(v0);
  if(v1->state & CTH_VARSTATE__FREEALL) cth_variable_destroy(v1);

  return NULL;
}


#endif
