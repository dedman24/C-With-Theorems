#ifndef SATISFIABLE_INTERPRETER_ARITH_H_INCLUDED
#define SATISFIABLE_INTERPRETER_ARITH_H_INCLUDED

// basic arithmetic operators '+', '-', '*', '/', '%'.

#include "../ctx.h"
#include "_primitives.h"
#include "../../common/_common.h"

static void satisfiable_op__add(
  satisfiable_interpreterT* const restrict interpreter,
  satisfiable_error_ctxT* const restrict err, rcT* const restrict path, const satisfiable_fposT line, const satisfiable_fposT offset
){
  satisfiable_variableT *restrict op0, *restrict op1;
  if(!satisfiable_op__setup_2(interpreter, &op0, &op1, err, path, line, offset))
    return;
  
  void* restrict raw_final;
  switch(op0->type){
    case SATISFIABLE_TYPE__I32:
      raw_final = malloc(sizeof(int32_t));
      *(int32_t*)raw_final = *(int32_t*)op0->internal + *(int32_t*)op1->internal;
    default:
      satisfiable_error_report__type1(SATISFIABLE_ERROR__INTERPRETER_TYPE_UNSUPPORTED, SATISFIABLE_ERROR__COMPILER, path, line, offset, err, op0->type);
  }

  const satisfiable_qualifierT qualif = satisfiable_qualifier__2(op0, op1);
  satisfiable_interpreter_push_variable(interpreter, NULL, 0, raw_final, op0->type, qualif);
}

// glorified noop.
static void satisfiable_op__pos(
  satisfiable_interpreterT* const restrict interpreter,
  satisfiable_error_ctxT* const restrict err, rcT* const restrict path, const satisfiable_fposT line, const satisfiable_fposT offset
){
  return;
}

#endif
