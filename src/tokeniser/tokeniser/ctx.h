#ifndef CTH_TOKENISER_CTX_H_INCLUDED
#define CTH_TOKENISER_CTX_H_INCLUDED

// tokeniser ctx.

#include "scope.h"

#include "../../common/tok.h"
#include "../../interpreter/ast.h"

typedef struct{
  cth_variable_ctxT* variables;       // variable data.
  cth_tokeniser_scopeT scope;         // scope data.
  vfastbst_strT immed;                // raw immediates, have no scope.
  vfastbst_rcT files;

  cth_fposT line;
  cth_fposT offset;
// e_state is the enum holding tokeniser state in cth.
// tokeniser is a state machine; perhaps this introduces technical debt, but I like it :DDD.
  enum{
    CTH_TOKENISER__NONE           =  0,
// rough diagram of state machine.
// CTH_TOKENISER__UNARY_ALLOWED --names-> CTH_TOKENISER__PREV_WAS_NAME
//              |               <--ops---                  ^ (through names)
//              \-closed bracket---> CTH_TOKENISER__NONE --/
    CTH_TOKENISER__PREV_WAS_NAME  = (1<<0),
    CTH_TOKENISER__UNARY_ALLOWED  = (1<<1),
    CTH_TOKENISER__FOR            = (1<<2),
    CTH_TOKENISER__WHILE          = (1<<3),
    CTH_TOKENISER__DECLARATION    = (1<<4),
    CTH_TOKENISER__TYPEDEF        = (1<<5),

    CTH_TOKENISER_STATE_INIT      = CTH_TOKENISER__UNARY_ALLOWED
  } e_state;
} cth_tokeniserT;

static void cth_tokeniser_init(cth_tokeniserT* const restrict tokeniser){
  cth_tokeniser_scope_init(&tokeniser->scope);
  tokeniser->immed = VFASTBST_STR_INIT;
  tokeniser->files = VFASTBST_RC_INIT;

  tokeniser->line = 0;
  tokeniser->offset = 0;

  tokeniser->e_state = CTH_TOKENISER_STATE_INIT;
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

static const char* cth_tokeniser_dispatch(
  const char* restrict code, const cth_tokT tok, const size_t len, cth_tokeniserT* const restrict tokeniser, 
  rcT* const restrict path, cth_error_ctxT* const restrict err
);

static void cth_tokeniser_dispatch__ast(
  const cth_tokT tok, cth_tokeniserT* const restrict tokeniser, 
  rcT* const restrict path, cth_error_ctxT* const restrict err
);

#endif
