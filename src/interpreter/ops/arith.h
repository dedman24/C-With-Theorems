#ifndef LTH_INTERPRETER_ARITH_H_INCLUDED
#define LTH_INTERPRETER_ARITH_H_INCLUDED

// basic arithmetic operators '+', '-', '*', '/', '%'.

#include "../ctx.h"
#include "_primitives.h"
#include "../../common/_common.h"

// stdlib includes.
#include "math.h"                 // fmod, fmodf.

static void lth_op__add(
  lth_interpreterT* const restrict interpreter,
  lth_error_ctxT* const restrict err, rcT* const restrict path, const lth_fposT line, const lth_fposT offset
){
  lth_variableT *restrict op0, *restrict op1;
  if(!lth_op__setup_2(interpreter, &op1, &op0, err, path, line, offset))
    return;

  void* const restrict raw_final =
    op0->type == LTH_TYPE__STRING? malloc(strlen(op0->internal) + strlen(op1->internal) + 1):
    malloc(lth_type__sizeof(op0));

  switch(op0->type){
    case LTH_TYPE__U8:  *(uint8_t*) raw_final = *(uint8_t*) op0->internal + *(uint8_t*) op1->internal; break;
    case LTH_TYPE__U16: *(uint16_t*)raw_final = *(uint16_t*)op0->internal + *(uint16_t*)op1->internal; break;
    case LTH_TYPE__U32: *(uint32_t*)raw_final = *(uint32_t*)op0->internal + *(uint32_t*)op1->internal; break;
    case LTH_TYPE__U64: *(uint64_t*)raw_final = *(uint64_t*)op0->internal + *(uint64_t*)op1->internal; break;

    case LTH_TYPE__I8:  *(int8_t*) raw_final = *(int8_t*) op0->internal + *(int8_t*) op1->internal; break;
    case LTH_TYPE__I16: *(int16_t*)raw_final = *(int16_t*)op0->internal + *(int16_t*)op1->internal; break;
    case LTH_TYPE__I32: *(int32_t*)raw_final = *(int32_t*)op0->internal + *(int32_t*)op1->internal; break;
    case LTH_TYPE__I64: *(int64_t*)raw_final = *(int64_t*)op0->internal + *(int64_t*)op1->internal; break;

    case LTH_TYPE__F32: *(float*) raw_final = *(float*) op0->internal + *(float*) op1->internal; break;
    case LTH_TYPE__F64: *(double*)raw_final = *(double*)op0->internal + *(double*)op1->internal; break;
// great deliberation on whether 'a' + 'b' should be "ab" or 
    case LTH_TYPE__CHAR:    *(char*)raw_final = *(char*)op0->internal + *(char*)op1->internal; break;
    case LTH_TYPE__BOOLEAN: *(bool*)raw_final = *(bool*)op0->internal ^ *(bool*)op1->internal; break;

    case LTH_TYPE__STRING:
      const size_t s_op0 = strlen(op0->internal);
      memcpy(raw_final, op0->internal, s_op0);
      strcpy(raw_final + s_op0, op1->internal);
      break;
      
    default:
      free(raw_final);
      lth_error_report__type1(LTH_ERROR__INTERPRETER_TYPE_UNSUPPORTED, LTH_ERROR__COMPILER, path, line, offset, err, op0->type);
  }

  const lth_qualifierT qualif = lth_qualifier__2(op0, op1);
  lth_interpreter_push_variable(interpreter, raw_final, op0->type, qualif);
}

// glorified noop.
static void lth_op__pos(
  lth_interpreterT* const restrict interpreter,
  lth_error_ctxT* const restrict err, rcT* const restrict path, const lth_fposT line, const lth_fposT offset
){
  return;
}

static void lth_op__sub(
  lth_interpreterT* const restrict interpreter,
  lth_error_ctxT* const restrict err, rcT* const restrict path, const lth_fposT line, const lth_fposT offset
){
  lth_variableT *restrict op0, *restrict op1;
  if(!lth_op__setup_2(interpreter, &op1, &op0, err, path, line, offset))
    return;

  void* const restrict raw_final = malloc(lth_type__sizeof(op0));

  switch(op0->type){
    case LTH_TYPE__U8:  *(uint8_t*) raw_final = *(uint8_t*) op0->internal - *(uint8_t*) op1->internal; break;
    case LTH_TYPE__U16: *(uint16_t*)raw_final = *(uint16_t*)op0->internal - *(uint16_t*)op1->internal; break;
    case LTH_TYPE__U32: *(uint32_t*)raw_final = *(uint32_t*)op0->internal - *(uint32_t*)op1->internal; break;
    case LTH_TYPE__U64: *(uint64_t*)raw_final = *(uint64_t*)op0->internal - *(uint64_t*)op1->internal; break;

    case LTH_TYPE__I8:  *(int8_t*) raw_final = *(int8_t*) op0->internal - *(int8_t*) op1->internal; break;
    case LTH_TYPE__I16: *(int16_t*)raw_final = *(int16_t*)op0->internal - *(int16_t*)op1->internal; break;
    case LTH_TYPE__I32: *(int32_t*)raw_final = *(int32_t*)op0->internal - *(int32_t*)op1->internal; break;
    case LTH_TYPE__I64: *(int64_t*)raw_final = *(int64_t*)op0->internal - *(int64_t*)op1->internal; break;

    case LTH_TYPE__F32: *(float*) raw_final = *(float*) op0->internal - *(float*) op1->internal; break;
    case LTH_TYPE__F64: *(double*)raw_final = *(double*)op0->internal - *(double*)op1->internal; break;

    case LTH_TYPE__CHAR:    *(char*)raw_final = *(char*)op0->internal - *(char*)op1->internal; break;
    case LTH_TYPE__BOOLEAN: *(bool*)raw_final = *(bool*)op0->internal ^ *(bool*)op1->internal; break;

    case LTH_TYPE__STRING:
      const size_t s_op0 = strlen(op0->internal);
      memcpy(raw_final, op0->internal, s_op0);
      strcpy(raw_final + s_op0, op1->internal);
      break;
      
    default:
      free(raw_final);
      lth_error_report__type1(LTH_ERROR__INTERPRETER_TYPE_UNSUPPORTED, LTH_ERROR__COMPILER, path, line, offset, err, op0->type);
  }

  const lth_qualifierT qualif = lth_qualifier__2(op0, op1);
  lth_interpreter_push_variable(interpreter, raw_final, op0->type, qualif);
}

static void lth_op__neg(
  lth_interpreterT* const restrict interpreter,
  lth_error_ctxT* const restrict err, rcT* const restrict path, const lth_fposT line, const lth_fposT offset
){
  lth_variableT *restrict op;
  if(!lth_op__setup_1(interpreter, &op, err, path, line, offset))
    return;

  void* const restrict raw_final = malloc(lth_type__sizeof(op));
  switch(op->type){
    case LTH_TYPE__U8:  *(uint8_t*) raw_final = -*(uint8_t*) op->internal; break;
    case LTH_TYPE__U16: *(uint16_t*)raw_final = -*(uint16_t*)op->internal; break;
    case LTH_TYPE__U32: *(uint32_t*)raw_final = -*(uint32_t*)op->internal; break;
    case LTH_TYPE__U64: *(uint64_t*)raw_final = -*(uint64_t*)op->internal; break;

    case LTH_TYPE__I8:  *(int8_t*) raw_final = -*(int8_t*) op->internal; break;
    case LTH_TYPE__I16: *(int16_t*)raw_final = -*(int16_t*)op->internal; break;
    case LTH_TYPE__I32: *(int32_t*)raw_final = -*(int32_t*)op->internal; break;
    case LTH_TYPE__I64: *(int64_t*)raw_final = -*(int64_t*)op->internal; break;

    case LTH_TYPE__F32: *(float*) raw_final = -*(float*) op->internal; break;
    case LTH_TYPE__F64: *(double*)raw_final = -*(double*)op->internal; break;

    case LTH_TYPE__CHAR:    *(char*)raw_final = -*(char*)op->internal; break;
    case LTH_TYPE__BOOLEAN: *(bool*)raw_final = *(bool*)op->internal ^ 1; break;

    default:
      free(raw_final);
      lth_error_report__type1(LTH_ERROR__INTERPRETER_TYPE_UNSUPPORTED, LTH_ERROR__COMPILER, path, line, offset, err, op->type);
  }

  const lth_qualifierT qualif = lth_qualifier__2(op, op);
  lth_interpreter_push_variable(interpreter, raw_final, op->type, qualif);
}

static void lth_op__mul(
  lth_interpreterT* const restrict interpreter,
  lth_error_ctxT* const restrict err, rcT* const restrict path, const lth_fposT line, const lth_fposT offset
){
  lth_variableT *restrict op0, *restrict op1;
  if(!lth_op__setup_2(interpreter, &op1, &op0, err, path, line, offset))
    return;

  void* const restrict raw_final = malloc(lth_type__sizeof(op0));
  switch(op0->type){
    case LTH_TYPE__U8:  *(uint8_t*) raw_final = *(uint8_t*) op0->internal * *(uint8_t*) op1->internal; break;
    case LTH_TYPE__U16: *(uint16_t*)raw_final = *(uint16_t*)op0->internal * *(uint16_t*)op1->internal; break;
    case LTH_TYPE__U32: *(uint32_t*)raw_final = *(uint32_t*)op0->internal * *(uint32_t*)op1->internal; break;
    case LTH_TYPE__U64: *(uint64_t*)raw_final = *(uint64_t*)op0->internal * *(uint64_t*)op1->internal; break;

    case LTH_TYPE__I8:  *(int8_t*) raw_final = *(int8_t*) op0->internal * *(int8_t*) op1->internal; break;
    case LTH_TYPE__I16: *(int16_t*)raw_final = *(int16_t*)op0->internal * *(int16_t*)op1->internal; break;
    case LTH_TYPE__I32: *(int32_t*)raw_final = *(int32_t*)op0->internal * *(int32_t*)op1->internal; break;
    case LTH_TYPE__I64: *(int64_t*)raw_final = *(int64_t*)op0->internal * *(int64_t*)op1->internal; break;

    case LTH_TYPE__F32: *(float*) raw_final = *(float*) op0->internal * *(float*) op1->internal; break;
    case LTH_TYPE__F64: *(double*)raw_final = *(double*)op0->internal * *(double*)op1->internal; break;

    case LTH_TYPE__CHAR:    *(char*)raw_final = *(char*)op0->internal * *(char*)op1->internal; break;
    case LTH_TYPE__BOOLEAN: *(bool*)raw_final = *(bool*)op0->internal & *(bool*)op1->internal; break;

    default:
      free(raw_final);
      lth_error_report__type1(LTH_ERROR__INTERPRETER_TYPE_UNSUPPORTED, LTH_ERROR__COMPILER, path, line, offset, err, op0->type);
  }

  const lth_qualifierT qualif = lth_qualifier__2(op0, op1);
  lth_interpreter_push_variable(interpreter, raw_final, op0->type, qualif);
}

static void lth_op__div(
  lth_interpreterT* const restrict interpreter,
  lth_error_ctxT* const restrict err, rcT* const restrict path, const lth_fposT line, const lth_fposT offset
){
  lth_variableT *restrict op0, *restrict op1;
  if(!lth_op__setup_2(interpreter, &op1, &op0, err, path, line, offset))
    return;

  void* const restrict raw_final = malloc(lth_type__sizeof(op0));
  switch(op0->type){
    case LTH_TYPE__U8:  if(*(uint8_t*) op1->internal == 0) goto DIVIDE_BY_ZERO; *(uint8_t*) raw_final = *(uint8_t*) op0->internal / *(uint8_t*) op1->internal; break;
    case LTH_TYPE__U16: if(*(uint16_t*)op1->internal == 0) goto DIVIDE_BY_ZERO; *(uint16_t*)raw_final = *(uint16_t*)op0->internal / *(uint16_t*)op1->internal; break;
    case LTH_TYPE__U32: if(*(uint32_t*)op1->internal == 0) goto DIVIDE_BY_ZERO; *(uint32_t*)raw_final = *(uint32_t*)op0->internal / *(uint32_t*)op1->internal; break;
    case LTH_TYPE__U64: if(*(uint64_t*)op1->internal == 0) goto DIVIDE_BY_ZERO; *(uint64_t*)raw_final = *(uint64_t*)op0->internal / *(uint64_t*)op1->internal; break;

    case LTH_TYPE__I8:  if(*(int8_t*) op1->internal == 0) goto DIVIDE_BY_ZERO; *(int8_t*) raw_final = *(int8_t*) op0->internal / *(int8_t*) op1->internal; break;
    case LTH_TYPE__I16: if(*(int16_t*)op1->internal == 0) goto DIVIDE_BY_ZERO; *(int16_t*)raw_final = *(int16_t*)op0->internal / *(int16_t*)op1->internal; break;
    case LTH_TYPE__I32: if(*(int32_t*)op1->internal == 0) goto DIVIDE_BY_ZERO; *(int32_t*)raw_final = *(int32_t*)op0->internal / *(int32_t*)op1->internal; break;
    case LTH_TYPE__I64: if(*(int64_t*)op1->internal == 0) goto DIVIDE_BY_ZERO; *(int64_t*)raw_final = *(int64_t*)op0->internal / *(int64_t*)op1->internal; break;

    case LTH_TYPE__F32: if(*(float*)op1->internal  == 0.0f) goto DIVIDE_BY_ZERO; *(float*) raw_final = *(float*) op0->internal / *(float*) op1->internal; break;
    case LTH_TYPE__F64: if(*(double*)op1->internal == 0.0)  goto DIVIDE_BY_ZERO; *(double*)raw_final = *(double*)op0->internal / *(double*)op1->internal; break;

    case LTH_TYPE__CHAR: if(*(char*) op1->internal == 0) goto DIVIDE_BY_ZERO; *(char*)raw_final = *(char*)op0->internal / *(char*)op1->internal; break;
    DIVIDE_BY_ZERO:
      free(raw_final);
      lth_error_report(LTH_ERROR__INTERPRETER_DIVIDE_BY_ZERO, LTH_ERROR__COMPILER, path, line, offset, err);
      break;
    default:
      free(raw_final);
      lth_error_report__type1(LTH_ERROR__INTERPRETER_TYPE_UNSUPPORTED, LTH_ERROR__COMPILER, path, line, offset, err, op0->type);
      break;
  }

  const lth_qualifierT qualif = lth_qualifier__2(op0, op1);
  lth_interpreter_push_variable(interpreter, raw_final, op0->type, qualif);
}

static void lth_op__mod(
  lth_interpreterT* const restrict interpreter,
  lth_error_ctxT* const restrict err, rcT* const restrict path, const lth_fposT line, const lth_fposT offset
){
  lth_variableT *restrict op0, *restrict op1;
  if(!lth_op__setup_2(interpreter, &op1, &op0, err, path, line, offset))
    return;

  void* const restrict raw_final = malloc(lth_type__sizeof(op0));
  switch(op0->type){
    case LTH_TYPE__U8:  if(*(uint8_t*) op1->internal == 0) goto DIVIDE_BY_ZERO; *(uint8_t*) raw_final = *(uint8_t*) op0->internal % *(uint8_t*) op1->internal; break;
    case LTH_TYPE__U16: if(*(uint16_t*)op1->internal == 0) goto DIVIDE_BY_ZERO; *(uint16_t*)raw_final = *(uint16_t*)op0->internal % *(uint16_t*)op1->internal; break;
    case LTH_TYPE__U32: if(*(uint32_t*)op1->internal == 0) goto DIVIDE_BY_ZERO; *(uint32_t*)raw_final = *(uint32_t*)op0->internal % *(uint32_t*)op1->internal; break;
    case LTH_TYPE__U64: if(*(uint64_t*)op1->internal == 0) goto DIVIDE_BY_ZERO; *(uint64_t*)raw_final = *(uint64_t*)op0->internal % *(uint64_t*)op1->internal; break;

    case LTH_TYPE__I8:  if(*(int8_t*) op1->internal == 0) goto DIVIDE_BY_ZERO; *(int8_t*) raw_final = *(int8_t*) op0->internal % *(int8_t*) op1->internal; break;
    case LTH_TYPE__I16: if(*(int16_t*)op1->internal == 0) goto DIVIDE_BY_ZERO; *(int16_t*)raw_final = *(int16_t*)op0->internal % *(int16_t*)op1->internal; break;
    case LTH_TYPE__I32: if(*(int32_t*)op1->internal == 0) goto DIVIDE_BY_ZERO; *(int32_t*)raw_final = *(int32_t*)op0->internal % *(int32_t*)op1->internal; break;
    case LTH_TYPE__I64: if(*(int64_t*)op1->internal == 0) goto DIVIDE_BY_ZERO; *(int64_t*)raw_final = *(int64_t*)op0->internal % *(int64_t*)op1->internal; break;

    case LTH_TYPE__F32: if(*(float*)op1->internal  == 0.0f) goto DIVIDE_BY_ZERO; *(float*) raw_final = fmodf(*(float*) op0->internal, *(float*) op1->internal); break;
    case LTH_TYPE__F64: if(*(double*)op1->internal == 0.0)  goto DIVIDE_BY_ZERO; *(double*)raw_final = fmod(*(double*)op0->internal,  *(double*)op1->internal); break;

    case LTH_TYPE__CHAR: if(*(char*) op1->internal == 0) goto DIVIDE_BY_ZERO; *(char*)raw_final = *(char*)op0->internal % *(char*)op1->internal; break;
    DIVIDE_BY_ZERO:
      free(raw_final);
      lth_error_report(LTH_ERROR__INTERPRETER_DIVIDE_BY_ZERO, LTH_ERROR__COMPILER, path, line, offset, err);
      break;
    default:
      free(raw_final);
      lth_error_report__type1(LTH_ERROR__INTERPRETER_TYPE_UNSUPPORTED, LTH_ERROR__COMPILER, path, line, offset, err, op0->type);
      break;
  }

  const lth_qualifierT qualif = lth_qualifier__2(op0, op1);
  lth_interpreter_push_variable(interpreter, raw_final, op0->type, qualif);
}

#endif
