#ifndef CTH_TOKENISER_CTX_H_INCLUDED
#define CTH_TOKENISER_CTX_H_INCLUDED

// tokeniser ctx fwd declarations.

#include "../../common/_common.h"
#include "../../interpreter/ast.h"

typedef struct cth_tokeniserS cth_tokeniserT;

static const char* cth_tokeniser_dispatch(
  const char* restrict code, const cth_tokT tok, const size_t len, cth_tokeniserT* const restrict tokeniser, 
  rcT* const restrict path, cth_error_ctxT* const restrict err
);

static void cth_tokeniser_dispatch__ast(
  const cth_tokT tok, cth_tokeniserT* const restrict tokeniser, 
  rcT* const restrict path, cth_error_ctxT* const restrict err
);

#endif
