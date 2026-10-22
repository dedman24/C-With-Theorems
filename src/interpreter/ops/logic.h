#ifndef LTH_INTERPRETER_LOGIC_H_INCLUDED
#define LTH_INTERPRETER_LOGIC_H_INCLUDED

// bitwise logic operators '&', '|', '^', '~'.
// short circuit logic operators '&&', '||', '^^', '!'.

#include "../ctx.h"
#include "_primitives.h"
#include "../../common/_common.h"

static void lth_op__and(
  lth_interpreterT* const restrict interpreter,
  lth_error_ctxT* const restrict err, rcT* const restrict path, const lth_fposT line, const lth_fposT offset
){
  lth_variableT *restrict op0, *restrict op1;
  if(!lth_op__setup_2(interpreter, &op1, &op0, err, path, line, offset))
    return;

  void* const restrict raw_final = malloc(lth_type__sizeof(op0));

  switch(op0->type){
    case LTH_TYPE__U8:  *(uint8_t*) raw_final = *(uint8_t*) op0->internal & *(uint8_t*) op1->internal; break;
    case LTH_TYPE__U16: *(uint16_t*)raw_final = *(uint16_t*)op0->internal & *(uint16_t*)op1->internal; break;
    case LTH_TYPE__U32: *(uint32_t*)raw_final = *(uint32_t*)op0->internal & *(uint32_t*)op1->internal; break;
    case LTH_TYPE__U64: *(uint64_t*)raw_final = *(uint64_t*)op0->internal & *(uint64_t*)op1->internal; break;

    case LTH_TYPE__I8:  *(int8_t*) raw_final = *(int8_t*) op0->internal & *(int8_t*) op1->internal; break;
    case LTH_TYPE__I16: *(int16_t*)raw_final = *(int16_t*)op0->internal & *(int16_t*)op1->internal; break;
    case LTH_TYPE__I32: *(int32_t*)raw_final = *(int32_t*)op0->internal & *(int32_t*)op1->internal; break;
    case LTH_TYPE__I64: *(int64_t*)raw_final = *(int64_t*)op0->internal & *(int64_t*)op1->internal; break;
// logic ops on floats are useful, especially when one's designing a CPU.
    case LTH_TYPE__F32: *(uint32_t*)raw_final = *(uint32_t*)op0->internal & *(uint32_t*)op1->internal; break;
    case LTH_TYPE__F64: *(uint64_t*)raw_final = *(uint64_t*)op0->internal & *(uint64_t*)op1->internal; break;

    case LTH_TYPE__CHAR:    *(char*)raw_final = *(char*)op0->internal & *(char*)op1->internal; break;
    case LTH_TYPE__BOOLEAN: *(bool*)raw_final = *(bool*)op0->internal & *(bool*)op1->internal; break;

    default:
      free(raw_final);
      lth_error_report__type1(LTH_ERROR__INTERPRETER_TYPE_UNSUPPORTED, LTH_ERROR__COMPILER, path, line, offset, err, op0->type);
  }

  const lth_qualifierT qualif = lth_qualifier__2(op0, op1);
  lth_interpreter_push_variable(interpreter, raw_final, op0->type, qualif);
}

static void lth_op__or(
  lth_interpreterT* const restrict interpreter,
  lth_error_ctxT* const restrict err, rcT* const restrict path, const lth_fposT line, const lth_fposT offset
){
  lth_variableT *restrict op0, *restrict op1;
  if(!lth_op__setup_2(interpreter, &op1, &op0, err, path, line, offset))
    return;

  void* const restrict raw_final = malloc(lth_type__sizeof(op0));

  switch(op0->type){
    case LTH_TYPE__U8:  *(uint8_t*) raw_final = *(uint8_t*) op0->internal | *(uint8_t*) op1->internal; break;
    case LTH_TYPE__U16: *(uint16_t*)raw_final = *(uint16_t*)op0->internal | *(uint16_t*)op1->internal; break;
    case LTH_TYPE__U32: *(uint32_t*)raw_final = *(uint32_t*)op0->internal | *(uint32_t*)op1->internal; break;
    case LTH_TYPE__U64: *(uint64_t*)raw_final = *(uint64_t*)op0->internal | *(uint64_t*)op1->internal; break;

    case LTH_TYPE__I8:  *(int8_t*) raw_final = *(int8_t*) op0->internal | *(int8_t*) op1->internal; break;
    case LTH_TYPE__I16: *(int16_t*)raw_final = *(int16_t*)op0->internal | *(int16_t*)op1->internal; break;
    case LTH_TYPE__I32: *(int32_t*)raw_final = *(int32_t*)op0->internal | *(int32_t*)op1->internal; break;
    case LTH_TYPE__I64: *(int64_t*)raw_final = *(int64_t*)op0->internal | *(int64_t*)op1->internal; break;

    case LTH_TYPE__F32: *(uint32_t*)raw_final = *(uint32_t*)op0->internal | *(uint32_t*)op1->internal; break;
    case LTH_TYPE__F64: *(uint64_t*)raw_final = *(uint64_t*)op0->internal | *(uint64_t*)op1->internal; break;

    case LTH_TYPE__CHAR:    *(char*)raw_final = *(char*)op0->internal | *(char*)op1->internal; break;
    case LTH_TYPE__BOOLEAN: *(bool*)raw_final = *(bool*)op0->internal | *(bool*)op1->internal; break;

    default:
      free(raw_final);
      lth_error_report__type1(LTH_ERROR__INTERPRETER_TYPE_UNSUPPORTED, LTH_ERROR__COMPILER, path, line, offset, err, op0->type);
  }

  const lth_qualifierT qualif = lth_qualifier__2(op0, op1);
  lth_interpreter_push_variable(interpreter, raw_final, op0->type, qualif);
}


static void lth_op__xor(
  lth_interpreterT* const restrict interpreter,
  lth_error_ctxT* const restrict err, rcT* const restrict path, const lth_fposT line, const lth_fposT offset
){
  lth_variableT *restrict op0, *restrict op1;
  if(!lth_op__setup_2(interpreter, &op1, &op0, err, path, line, offset))
    return;

  void* const restrict raw_final = malloc(lth_type__sizeof(op0));

  switch(op0->type){
    case LTH_TYPE__U8:  *(uint8_t*) raw_final = *(uint8_t*) op0->internal ^ *(uint8_t*) op1->internal; break;
    case LTH_TYPE__U16: *(uint16_t*)raw_final = *(uint16_t*)op0->internal ^ *(uint16_t*)op1->internal; break;
    case LTH_TYPE__U32: *(uint32_t*)raw_final = *(uint32_t*)op0->internal ^ *(uint32_t*)op1->internal; break;
    case LTH_TYPE__U64: *(uint64_t*)raw_final = *(uint64_t*)op0->internal ^ *(uint64_t*)op1->internal; break;

    case LTH_TYPE__I8:  *(int8_t*) raw_final = *(int8_t*) op0->internal ^ *(int8_t*) op1->internal; break;
    case LTH_TYPE__I16: *(int16_t*)raw_final = *(int16_t*)op0->internal ^ *(int16_t*)op1->internal; break;
    case LTH_TYPE__I32: *(int32_t*)raw_final = *(int32_t*)op0->internal ^ *(int32_t*)op1->internal; break;
    case LTH_TYPE__I64: *(int64_t*)raw_final = *(int64_t*)op0->internal ^ *(int64_t*)op1->internal; break;

    case LTH_TYPE__F32: *(uint32_t*)raw_final = *(uint32_t*)op0->internal ^ *(uint32_t*)op1->internal; break;
    case LTH_TYPE__F64: *(uint64_t*)raw_final = *(uint64_t*)op0->internal ^ *(uint64_t*)op1->internal; break;

    case LTH_TYPE__CHAR:    *(char*)raw_final = *(char*)op0->internal ^ *(char*)op1->internal; break;
    case LTH_TYPE__BOOLEAN: *(bool*)raw_final = *(bool*)op0->internal ^ *(bool*)op1->internal; break;

    default:
      free(raw_final);
      lth_error_report__type1(LTH_ERROR__INTERPRETER_TYPE_UNSUPPORTED, LTH_ERROR__COMPILER, path, line, offset, err, op0->type);
  }

  const lth_qualifierT qualif = lth_qualifier__2(op0, op1);
  lth_interpreter_push_variable(interpreter, raw_final, op0->type, qualif);
}

static void lth_op__not(
  lth_interpreterT* const restrict interpreter,
  lth_error_ctxT* const restrict err, rcT* const restrict path, const lth_fposT line, const lth_fposT offset
){
  lth_variableT *restrict op;
  if(!lth_op__setup_1(interpreter, &op, err, path, line, offset))
    return;

  void* const restrict raw_final = malloc(lth_type__sizeof(op));
  switch(op->type){
    case LTH_TYPE__U8:  *(uint8_t*) raw_final = ~*(uint8_t*) op->internal; break;
    case LTH_TYPE__U16: *(uint16_t*)raw_final = ~*(uint16_t*)op->internal; break;
    case LTH_TYPE__U32: *(uint32_t*)raw_final = ~*(uint32_t*)op->internal; break;
    case LTH_TYPE__U64: *(uint64_t*)raw_final = ~*(uint64_t*)op->internal; break;

    case LTH_TYPE__I8:  *(int8_t*) raw_final = ~*(int8_t*) op->internal; break;
    case LTH_TYPE__I16: *(int16_t*)raw_final = ~*(int16_t*)op->internal; break;
    case LTH_TYPE__I32: *(int32_t*)raw_final = ~*(int32_t*)op->internal; break;
    case LTH_TYPE__I64: *(int64_t*)raw_final = ~*(int64_t*)op->internal; break;

    case LTH_TYPE__F32: *(uint32_t*)raw_final = ~*(uint32_t*)op->internal; break;
    case LTH_TYPE__F64: *(uint64_t*)raw_final = ~*(uint64_t*)op->internal; break;

    case LTH_TYPE__CHAR:    *(char*)raw_final = ~*(char*)op->internal; break;
    case LTH_TYPE__BOOLEAN: *(bool*)raw_final = ~*(bool*)op->internal; break;
      
    default:
      free(raw_final);
      lth_error_report__type1(LTH_ERROR__INTERPRETER_TYPE_UNSUPPORTED, LTH_ERROR__COMPILER, path, line, offset, err, op->type);
  }

  const lth_qualifierT qualif = lth_qualifier__2(op, op);
  lth_interpreter_push_variable(interpreter, raw_final, op->type, qualif);
}

#endif
