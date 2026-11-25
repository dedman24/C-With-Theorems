#ifndef CTH_TOKENISER_TOKENISER_H_INCLUDED
#define CTH_TOKENISER_TOKENISER_H_INCLUDED

// all data about the tokeniser ctx & ops.

#include "ctx.h"
#include "name.h"
#include "scope.h"
#include "ctx_full.h"

static void cth_tokeniser_init(cth_tokeniserT* const restrict tokeniser){
  cth_tokeniser_scope_init(&tokeniser->scope);
  tokeniser->variables = cth_variable_ctx_init(NULL);
  tokeniser->immed = VFASTBST_STR_INIT;
  tokeniser->files = VFASTBST_RC_INIT;

  tokeniser->line = 0;
  tokeniser->offset = 0;
}

// tokeniser ctx always assumed to be statically allocated.
// tokeniser ~ must not be NULL.
// variables ~ may be NULL.
// err ~ may be NULL.
// path ~ may be NULL.
static cth_outputT cth_tokeniser_destroy(
  cth_tokeniserT* const restrict tokeniser,  cth_variable_ctxT** variables,
  cth_error_ctxT* const restrict err, rcT* const restrict path
){
  if(variables) *variables = tokeniser->variables;
  else cth_variable_ctx_destroy(tokeniser->variables, true);

  vfastbst_str_destroy(&tokeniser->immed);
  vfastbst_rc_destroy(&tokeniser->files);
  return cth_tokeniser_scope_destroy(&tokeniser->scope);
}

#endif
