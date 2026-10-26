#ifndef CTH_INTERPRETER_CTX_H_INCLUDED
#define CTH_INTERPRETER_CTX_H_INCLUDED

#include "ast.h"
#include "../common/err/err.h"

static void* cth_interpret(const cth_astT* const restrict program, cth_variable_ctxT* const restrict variables, rcT* const restrict path, cth_error_ctxT* const restrict err);

#endif
