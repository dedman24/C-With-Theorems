#ifndef CTH_TOKENISER_OPS_DECL_H_INCLUDED
#define CTH_TOKENISER_OPS_DECL_H_INCLUDED

#include "../ctx.h"

// declaration handling was a mess; this function streamlines all of it.

// handles declarations & their syntax.
static const char* cth_tokeniser_op__declaration(
  cth_tokeniserT* const restrict tokeniser, const char* restrict code, const size_t len,
  rcT* const restrict path, cth_error_ctxT* const restrict err
){
  return code;
}


#endif
