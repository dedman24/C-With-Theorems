#ifndef CTH_TOKENISER_CTX_H_INCLUDED
#define CTH_TOKENISER_CTX_H_INCLUDED

// tokeniser ctx.

#include "name.h"
#include "../common/tok.h"
#include "../interpreter/ast.h"

typedef struct{
  cth_tokT* ops;
  cth_nameT* values;
  cth_astT* program;

  cth_name_ctxT name;
  vfastbst_rcT files;

  cth_fposT line;
  cth_fposT offset;
// tokeniser is a state machine; perhaps this introduces technical debt, but I like it :DDD.
  enum{
  // example of states; TODO: implement actual state stuff.
    CTH_TOKENISER__NONE           =  0,
    CTH_TOKENISER__PREV_WAS_NAME  = (1<<0),
    CTH_TOKENISER__FOR            = (1<<1),
    CTH_TOKENISER__WHILE          = (1<<2),
    CTH_TOKENISER__DECLARATION    = (1<<3)
  } state;
// data in state.
  struct{
  } d_state;
} cth_tokeniserT;

static void cth_tokeniser_init(cth_tokeniserT* const restrict tokeniser){
  tokeniser->ops = vfastarr_init(sizeof(*tokeniser->ops));
  tokeniser->values = vfastarr_init(sizeof(*tokeniser->values));
// sets tokeniser->program to this node.
  tokeniser->program = cth_ast_init(CTH_TOK__NONE, 0, 0, CTH_AST__PROGRAM);

  cth_name_ctx_init(&tokeniser->name);
  tokeniser->files = VFASTBST_RC_INIT;

  tokeniser->line = 0;
  tokeniser->offset = 0;

  tokeniser->state = CTH_TOKENISER__NONE;
// doesn't really have to be initialised.
//  tokeniser->type = 0;
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
  if(variables) *variables = tokeniser->name.variables;
  else cth_variable_ctx_destroy(tokeniser->name.variables, true);

  if(!cth_name_ctx_destroy(&tokeniser->name, false) && err)
    cth_error_report(CTH_ERROR__INDENTATION_DEPTH_NEGATIVE, CTH_ERROR__COMPILER_DESTRUCTION, path, 0, 0, err);
  vfastarr_destroy(tokeniser->ops);
  vfastarr_destroy(tokeniser->values);
  vfastbst_rc_destroy(&tokeniser->files);

  return (cth_outputT)tokeniser->program;
}

static const char* cth_tokeniser_dispatch(
  const char* restrict code, const cth_tokT tok, const size_t len, cth_tokeniserT* const restrict tokeniser, 
  rcT* const restrict path, cth_error_ctxT* const restrict err
);

#endif
