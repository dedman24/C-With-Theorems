#ifndef LTH_INTERPRETER__OPS_H_INCLUDED
#define LTH_INTERPRETER__OPS_H_INCLUDED

#include "../ctx.h"
#include "../../common/_common.h"

#include "arith.h"
#include "logic.h"

typedef void (*lth_interpreter_opT)(
  lth_interpreterT* const restrict interpreter,
  lth_error_ctxT* const restrict err, rcT* const restrict path, const lth_fposT line, const lth_fposT offset
);

// operation array.
static lth_interpreter_opT lth_interpreter_ops[][2] = {

};

#endif
