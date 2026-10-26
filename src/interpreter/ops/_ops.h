#ifndef CTH_INTERPRETER_OPS_H_INCLUDED
#define CTH_INTERPRETER_OPS_H_INCLUDED

// all cth ops.

#include "misc.h"
#include "binary.h"

typedef cth_variableT* (*cth_ast__functionT)(const cth_astT* const restrict node, cth_variable_ctxT* const restrict variables, rcT* const restrict path, cth_error_ctxT* const restrict err);

const cth_ast__functionT cth_ast__functions[] = {
  [CTH_TOK__NAME] = cth_op__name,

  [CTH_TOK__PLUS] = cth_op__add,
};

#endif
