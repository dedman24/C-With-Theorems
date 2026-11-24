#ifndef CTH_TOKENISER_H_INCLUDED
#define CTH_TOKENISER_H_INCLUDED

#include "ops/_ops.h"               // tokeniser ops.
#include "tokeniser/ctx.h"          // interpreter ctx.
#include "../common/_common.h"      // tokens et cetera.

// stdlib includes.
#include "stddef.h"                 // for size_t.
#include "string.h"                 // for string manipulation functions.

// path   ~ file path itself.
// s_path ~ length of path excluding actual filename.
// err    ~ error ctx struct holding all reported during compilation.
static void cth_tokenise__single(
  const char* restrict code, cth_tokeniserT* const restrict ctx,
  rcT* const restrict path, cth_error_ctxT* const restrict err
);

// tokenises code pointed at by 'code' variable.
static void cth_tokenise__single(
  const char* restrict code, cth_tokeniserT* const restrict tokeniser, 
  rcT* const restrict path, cth_error_ctxT* const restrict err
){
  while(*code){
    size_t len;
    const cth_tokT tok = cth_tok(code, &len);
    code = cth_tokeniser_dispatch(code, tok, len, tokeniser, path, err);
  }
}

static cth_outputT cth_tokenise(
  const char* restrict code, const char* const restrict path,
  cth_variable_ctxT** const restrict variables, cth_error_ctxT* const restrict err
){
  rcT* const restrict rc_path = rc_make(path);
  cth_tokeniserT tokeniser; cth_tokeniser_init(&tokeniser);

  cth_tokenise__single(code, &tokeniser, rc_path, err);
  cth_outputT tokenised = cth_tokeniser_destroy(&tokeniser, variables, err, rc_path);
  rc_disown(rc_path);
// result of tokenisation.
  return tokenised;
}

// tokeniser function to use specifically with the REPL.
// accepts explicit tokeniser ctx, doesn't destroy it.
// invalidates tokenised still.
static cth_outputT cth_tokenise__repl(
  const char* restrict code, const char* const restrict path, 
  cth_tokeniserT* const restrict tokeniser, cth_error_ctxT* const restrict err
){
  rcT* const restrict rc_path = rc_make(path);

  cth_tokenise__single(code, tokeniser, rc_path, err);
  rc_disown(rc_path);

  return cth_tokeniser_scope_program_extract(&tokeniser->scope);
}

#endif
