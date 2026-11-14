#ifndef CTH_COMMON_VARIABLES_BASICS_H_INCLUDED
#define CTH_COMMON_VARIABLES_BASICS_H_INCLUDED

#include "ctx.h"
#include "types.h"

#define CTH_NAME__NULL (CTH_QUALIFIER_MAX)
#define CTH_NAME__FALSE (CTH_NAME__NULL+2)
#define CTH_NAME__TRUE (CTH_NAME__FALSE+2)
#define CTH_NAME__ID_START (CTH_NAME__TRUE+2)

#define CTH_NULL__UNDERLYING ((uintptr_t)0)

const char* const restrict cth_variable_name__NULL = "NULL";
const char* const restrict cth_variable_name__true = "true";
const char* const restrict cth_variable_name__false = "false";

cth_variableT* restrict cth_variable__NULL;
cth_variableT* restrict cth_variable__true;
cth_variableT* restrict cth_variable__false;

static void cth_variable_ctx_init__basic(cth_variable_ctxT* const restrict ctx){
  {
    cth_variable__NULL = cth_variable_add(ctx, 
      CTH_NAME__NULL, cth_variable_name__NULL, strlen(cth_variable_name__NULL),
      cth_typevar(CTH_TYPE__NULLPTR), CTH_VARSTATE__IMMEDIATE
    );
    cth_variable__NULL->internal.ptr = CTH_NULL__UNDERLYING;
  }
  {
    cth_variable__true = cth_variable_add(ctx, 
      CTH_NAME__TRUE, cth_variable_name__true, strlen(cth_variable_name__true),
      cth_typevar(CTH_TYPE__BOOLEAN), CTH_VARSTATE__IMMEDIATE
    );
    cth_variable__true->internal.boolean = true;
  }
  {
    cth_variable__false = cth_variable_add(ctx, 
      CTH_NAME__FALSE, cth_variable_name__false, strlen(cth_variable_name__false),
      cth_typevar(CTH_TYPE__BOOLEAN), CTH_VARSTATE__IMMEDIATE
    );
    cth_variable__false->internal.boolean = false;
  }
}

#endif
