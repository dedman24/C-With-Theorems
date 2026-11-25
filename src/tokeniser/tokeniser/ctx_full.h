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
};

#endif
