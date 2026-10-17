#ifndef SATISFIABLE_INTERPRETER__OPS_H_INCLUDED
#define SATISFIABLE_INTERPRETER__OPS_H_INCLUDED

#include "../ctx.h"
#include "../../common/_common.h"

#include "arith.h"

typedef void (*satisfiable_interpreter_opT)(
  satisfiable_interpreterT* const restrict interpreter,
  satisfiable_error_ctxT* const restrict err, rcT* const restrict path, const satisfiable_fposT line, const satisfiable_fposT offset
);

// operation array.
static satisfiable_interpreter_opT satisfiable_interpreter_ops[][2] = {
  [SATISFIABLE_TOK__PLUS] = { satisfiable_op__add, satisfiable_op__pos }
};

#endif
