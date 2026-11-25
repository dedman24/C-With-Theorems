#ifndef CTH_TOKENISER_CTX_FULL_H_INCLUDED
#define CTH_TOKENISER_CTX_FULL_H_INCLUDED

// ughhh I hate C so much I love it.

#include "ctx.h"
#include "scope.h"

struct cth_tokeniserS{
  cth_variable_ctxT* variables;       // variable data.
  cth_tokeniser_scopeT scope;         // scope data.
  vfastbst_strT immed;                // raw immediates, have no scope.
  vfastbst_rcT files;

  cth_fposT line;
  cth_fposT offset;
};

#endif
