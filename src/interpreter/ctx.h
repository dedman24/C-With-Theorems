#ifndef CTH_INTERPRETER_CTX_H_INCLUDED
#define CTH_INTERPRETER_CTX_H_INCLUDED

#include "ast.h"

typedef struct{

} cth_interpreterT;

static void cth_interpret(const cth_ast_programT* const restrict program);
static cth_nameT cth_ast_simplify(const cth_astT* const restrict node, cth_interpreterT* const restrict interpreter);

#endif
