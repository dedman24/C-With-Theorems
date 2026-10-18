#ifndef SATISFIABLE_OPS__PRIMITIVES_H_INCLUDED
#define SATISFIABLE_OPS__PRIMITIVES_H_INCLUDED

#include "../ctx.h"
#include "../../common/_common.h"

// stdlib includes.
#include "stdbool.h"            // bool.

static bool satisfiable_op__setup_2(
  satisfiable_interpreterT* const restrict interpreter,
  satisfiable_variableT* restrict* const restrict op0, satisfiable_variableT* restrict* const restrict op1,
  satisfiable_error_ctxT* const restrict err, rcT* const restrict path, const satisfiable_fposT line, const satisfiable_fposT offset
){
  *op0 = satisfiable_interpreter_pop_variable(interpreter, err, path, line, offset);
  *op1 = satisfiable_interpreter_pop_variable(interpreter, err, path, line, offset);

  if(!(*op0) || !(*op1)){
    satisfiable_error_report__u32(SATISFIABLE_ERROR__IMPOSSIBILITY, SATISFIABLE_ERROR__COMPILER, path, line, offset, err, SATISFIABLE_IMPOSSIBILITY__OP_TYPE);
    return false;
  }

  if((*op0)->type == SATISFIABLE_TYPE__UNDEFINED || (*op1)->type == SATISFIABLE_TYPE__UNDEFINED){
    satisfiable_error_report(SATISFIABLE_ERROR__VALUE_UNASSIGNED, SATISFIABLE_ERROR__COMPILER, path, line, offset, err);
    return false;
  }

  if((*op0)->type == SATISFIABLE_TYPE__NONE || (*op1)->type == SATISFIABLE_TYPE__NONE){
    if((*op0)->type == (*op1)->type)      // both being errors, no 'correct' type can be figured out.
      satisfiable_interpreter_push_value(interpreter, SATISFIABLE_VARIABLE__ID_ERROR);
    else{
      const satisfiable_typeT dst_type = (*op0)->type != SATISFIABLE_TYPE__NONE?
        (*op0)->type:
        (*op1)->type;
      satisfiable_interpreter_push_variable(interpreter, NULL, 0, NULL, dst_type, SATISFIABLE_QUALIFIER__NONE);
    }
    return false;
  }

  if((*op0)->type != (*op1)->type){       // type errors, how exciting!
    satisfiable_error_report__type2(SATISFIABLE_ERROR__TYPE_INCOMPATIBLE_OP, SATISFIABLE_ERROR__COMPILER, path, line, offset, err, (*op0)->type, (*op1)->type);
  // if we don't push anything to the stack, truly horrible stuff can happen, so we add a value of type NULL, holding nothing.
    satisfiable_interpreter_push_value(interpreter, SATISFIABLE_VARIABLE__ID_ERROR);
    return false;
  }

  if(!(*op0)->internal || !(*op1)->internal){
    satisfiable_error_report(SATISFIABLE_ERROR__VALUE_UNASSIGNED, SATISFIABLE_ERROR__COMPILER, path, line, offset, err);
    return false;
  }

  return true;
}

static satisfiable_qualifierT satisfiable_qualifier__2(const satisfiable_variableT* const restrict op0, const satisfiable_variableT* const restrict op1){
  return op0->qualifier & op1->qualifier & SATISFIABLE_QUALIFIER__CONST;      // some day add more.
}

#endif
