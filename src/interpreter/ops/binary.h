#ifndef CTH_INTERPRETER_OPS_BINARY_H_INCLUDED
#define CTH_INTERPRETER_OPS_BINARY_H_INCLUDED

// arithmetic cth ops.

#include "../ctx.h"                     // ast, ctx stuff.
#include "typecheck.h"                  // typechecking ops.
#include "../../common/_common.h"       // variableT & more.

static void cth_op__add_ptr(cth_variableT* const ptr, cth_variableT* const value){
  switch(value->type){
    case CTH_TYPE__I8:  ptr->internal.ptr += value->internal.i8;  break;
    case CTH_TYPE__I16: ptr->internal.ptr += value->internal.i16; break;
    case CTH_TYPE__I32: ptr->internal.ptr += value->internal.i32; break;
    case CTH_TYPE__I64: ptr->internal.ptr += value->internal.i64; break;

    case CTH_TYPE__U8:  ptr->internal.ptr += value->internal.u8;  break;
    case CTH_TYPE__U16: ptr->internal.ptr += value->internal.u16; break;
    case CTH_TYPE__U32: ptr->internal.ptr += value->internal.u32; break;
    case CTH_TYPE__U64: ptr->internal.ptr += value->internal.u64; break;
  }
}

static cth_variableT* cth_op__add(const cth_astT* const restrict node, cth_variable_ctxT* const restrict variables, rcT* const restrict path, cth_error_ctxT* const restrict err){
  if(node->type != CTH_AST__BINARY){
    cth_error_report__u32_2(CTH_ERROR__INTERPRETER_AST_TYPE, CTH_ERROR__COMPILER, path, node->line, node->offset, err, node->type, CTH_AST__BINARY);
    return NULL;
  }

  cth_variableT *const v0 = cth_interpret(node->data._binary.s0, variables, path, err),
                *const v1 = cth_interpret(node->data._binary.s1, variables, path, err);
  if(!v0 || !v1) return NULL;

  cth_nameT dispatch;
  if(!cth_typecheck_compatible((cth_nameT[]){v0->type, v1->type}, 2, node->op, &dispatch, variables)){
    cth_error_report__type_2(CTH_ERROR__TYPE_INCOMPATIBLE_OP, CTH_ERROR__COMPILER, path, node->line, node->offset, err, variables, v0->type, v1->type);
    return NULL;
  }

  cth_variableT* const restrict result = cth_variable_init(NULL, 0, v1->type, cth_qualifier_op2(v0->state, v1->state) | CTH_VARSTATE__FREEALL);  
  switch(dispatch){
    case CTH_TYPE__I8:  
    case CTH_TYPE__I16:
    case CTH_TYPE__I32: result->internal.i32 = v0->internal.i32 + v1->internal.i32; break;
    case CTH_TYPE__I64: result->internal.i64 = v0->internal.i64 + v1->internal.i64; break;

    case CTH_TYPE__U8:
    case CTH_TYPE__U16:
    case CTH_TYPE__U32:
    case CTH_TYPE__U64:

    case CTH_TYPE__F8:
    case CTH_TYPE__F16:
    case CTH_TYPE__F32:
    case CTH_TYPE__F64:

    case CTH_TYPE__CHAR8:
    case CTH_TYPE__BOOLEAN:

    case CTH_QUALIFIER__PTR:
    case CTH_QUALIFIER__REF:
      cth_op__add_ptr(v1, v0);
  }

  if(v0->state & CTH_VARSTATE__FREEALL) cth_variable_destroy(v0);
  if(v1->state & CTH_VARSTATE__FREEALL) cth_variable_destroy(v1);

  return result;
}

#endif
