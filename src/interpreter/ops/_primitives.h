#ifndef LTH_OPS__PRIMITIVES_H_INCLUDED
#define LTH_OPS__PRIMITIVES_H_INCLUDED

#include "../ctx.h"
#include "../../common/_common.h"

// stdlib includes.
#include "stdbool.h"            // bool.

static bool lth_op__setup_2(
  lth_interpreterT* const restrict interpreter,
  lth_variableT* restrict* const restrict op1, lth_variableT* restrict* const restrict op0,
  lth_error_ctxT* const restrict err, rcT* const restrict path, const lth_fposT line, const lth_fposT offset
){
  *op1 = lth_interpreter_pop_variable(interpreter, err, path, line, offset);
  *op0 = lth_interpreter_pop_variable(interpreter, err, path, line, offset);

  if(!(*op0) || !(*op1)){
    lth_error_report__u32(LTH_ERROR__IMPOSSIBILITY, LTH_ERROR__COMPILER, path, line, offset, err, LTH_IMPOSSIBILITY__OP_TYPE);
    return false;
  }

  if((*op0)->type == LTH_TYPE__UNDEFINED || (*op1)->type == LTH_TYPE__UNDEFINED){
    lth_error_report(LTH_ERROR__VALUE_UNASSIGNED, LTH_ERROR__COMPILER, path, line, offset, err);
    return false;
  }

  if((*op0)->type == LTH_TYPE__NONE || (*op1)->type == LTH_TYPE__NONE){
    if((*op0)->type == (*op1)->type)      // both being errors, no 'correct' type can be figured out.
      lth_interpreter_push(interpreter, LTH_VARIABLE__ID_ERROR);
    else{
      const lth_nameT dst_type = (*op0)->type != LTH_TYPE__NONE?
        (*op0)->type:
        (*op1)->type;
      lth_interpreter_push_variable(interpreter, NULL, dst_type, LTH_QUALIFIER__NONE);
    }
    return false;
  }

  if((*op0)->type != (*op1)->type){       // type errors, how exciting!
    lth_error_report__type2(LTH_ERROR__TYPE_INCOMPATIBLE_OP, LTH_ERROR__COMPILER, path, line, offset, err, (*op0)->type, (*op1)->type);
  // if we don't push anything to the stack, truly horrible stuff can happen, so we add a value of type NULL, holding nothing.
    lth_interpreter_push(interpreter, LTH_VARIABLE__ID_ERROR);
    return false;
  }

  if(!(*op0)->internal || !(*op1)->internal){
    lth_error_report(LTH_ERROR__VALUE_UNASSIGNED, LTH_ERROR__COMPILER, path, line, offset, err);
    return false;
  }

  return true;
}


static bool lth_op__setup_1(
  lth_interpreterT* const restrict interpreter,
  lth_variableT* restrict* const restrict op,
  lth_error_ctxT* const restrict err, rcT* const restrict path, const lth_fposT line, const lth_fposT offset
){
  *op = lth_interpreter_pop_variable(interpreter, err, path, line, offset);
// should never happen.
  if(!(*op)){
    lth_error_report__u32(LTH_ERROR__IMPOSSIBILITY, LTH_ERROR__COMPILER, path, line, offset, err, LTH_IMPOSSIBILITY__OP_TYPE);
    return false;
  }
// undefined variable.
  if((*op)->type == LTH_TYPE__UNDEFINED){
    lth_error_report(LTH_ERROR__VALUE_UNASSIGNED, LTH_ERROR__COMPILER, path, line, offset, err);
    return false;
  }
// error type.
  if((*op)->type == LTH_TYPE__NONE){
    lth_interpreter_push(interpreter, LTH_VARIABLE__ID_ERROR);
    return false;
  }
// unassigned variable.
  if(!(*op)->internal){
    lth_error_report(LTH_ERROR__VALUE_UNASSIGNED, LTH_ERROR__COMPILER, path, line, offset, err);
    return false;
  }

  return true;
}

static lth_qualifierT lth_qualifier__2(const lth_variableT* const restrict op0, const lth_variableT* const restrict op1){
  return op0->qualifier & op1->qualifier & LTH_QUALIFIER__CONST;      // some day add more.
}

static lth_qualifierT lth_qualifier__1(const lth_variableT* const restrict op){
  return op->qualifier & LTH_QUALIFIER__CONST;
}

#endif
