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
//                                  unary version        binary version.
  [LTH_TOK__PLUS]       = { lth_op__pos, lth_op__add },
  [LTH_TOK__MINUS]      = { lth_op__neg, lth_op__sub },
  [LTH_TOK__STAR]       = { NULL,                lth_op__mul },
  [LTH_TOK__SLASH]      = { NULL,                lth_op__div },
  [LTH_TOK__PERCENT]    = { NULL,                lth_op__mod },
  [LTH_TOK__AMPERSAND]  = { NULL,                lth_op__and },
  [LTH_TOK__BAR]        = { NULL,                lth_op__or  },
  [LTH_TOK__CARAT]      = { NULL,                lth_op__xor },
  [LTH_TOK__TILDE]      = { lth_op__not, NULL },

};

#endif
