#ifndef CTH_INTERPRETER_OPS_H_INCLUDED
#define CTH_INTERPRETER_OPS_H_INCLUDED

// all cth ops.

#include "xof.h"
#include "decl.h"
#include "misc.h"
#include "binary.h"
#include "assignment.h"
#include "short-circuit.h"

typedef cth_variableT* (*cth_ast__functionT)(const cth_astT* const restrict node, cth_variable_ctxT* const restrict variables, rcT* const restrict path, cth_error_ctxT* const restrict err);

const cth_ast__functionT cth_ast__functions[] = {
  [CTH_TOK__NAME]         = cth_op__name,

  [CTH_TOK__COMMA]        = cth_op__comma,
  [CTH_TOK__TYPEIS]       = cth_op__typechecking,
  [CTH_TOK__ASSIGNMENT]   = cth_op__assignment,

  [CTH_TOK__ORL]          = cth_op__orl,
  [CTH_TOK__XORL]         = cth_op__xorl,
  [CTH_TOK__ANDL]         = cth_op__andl,

  [CTH_TOK__PLUS]         = cth_op__add,

  [CTH_TOK__UNARY_TYPEOF] = cth_op__typeof,
};

#endif
