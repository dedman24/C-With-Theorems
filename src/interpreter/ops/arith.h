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

  void* const restrict raw_final = malloc(satisfiable_type__sizeof(op0));
  switch(op0->type){
    case SATISFIABLE_TYPE__U8:  *(uint8_t*) raw_final = *(uint8_t*) op0->internal + *(uint8_t*) op1->internal; break;
    case SATISFIABLE_TYPE__U16: *(uint16_t*)raw_final = *(uint16_t*)op0->internal + *(uint16_t*)op1->internal; break;
    case SATISFIABLE_TYPE__U32: *(uint32_t*)raw_final = *(uint32_t*)op0->internal + *(uint32_t*)op1->internal; break;
    case SATISFIABLE_TYPE__U64: *(uint64_t*)raw_final = *(uint64_t*)op0->internal + *(uint64_t*)op1->internal; break;

    case SATISFIABLE_TYPE__I8:  *(int8_t*) raw_final = *(int8_t*) op0->internal + *(int8_t*) op1->internal; break;
    case SATISFIABLE_TYPE__I16: *(int16_t*)raw_final = *(int16_t*)op0->internal + *(int16_t*)op1->internal; break;
    case SATISFIABLE_TYPE__I32: *(int32_t*)raw_final = *(int32_t*)op0->internal + *(int32_t*)op1->internal; break;
    case SATISFIABLE_TYPE__I64: *(int64_t*)raw_final = *(int64_t*)op0->internal + *(int64_t*)op1->internal; break;


    case SATISFIABLE_TYPE__F32: *(float*) raw_final = *(float*) op0->internal + *(float*) op1->internal; break;
    case SATISFIABLE_TYPE__F64: *(double*)raw_final = *(double*)op0->internal + *(double*)op1->internal; break;
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
