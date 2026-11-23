#ifndef CTH_INTERPRETER_H_INCLUDED
#define CTH_INTERPRETER_H_INCLUDED

#include "ast.h"                  // abstract syntax tree stuff.
#include "ctx.h"
#include "ops/_ops.h"             // operations.
#include "../common/_common.h"    // variable ctx & more.

// recursively interprets ast pointed by program.
static void* cth_interpret(const cth_astT* const restrict node, cth_variable_ctxT* const restrict variables, rcT* const restrict path, cth_error_ctxT* const restrict err){
  switch(node->type){
    case CTH_AST__PROGRAM:
      const size_t c_program = vfastarr_elcnt(node->data.program.program, sizeof(*node->data.program.program));
    // temporary measure: a program's output is what each line evaluates to. useful for current REPL, little else. perhaps make it return NULL.
      for(size_t i = 0; i < c_program; i++){
        cth_variableT* result = cth_interpret(node->data.program.program[i], variables, path, err);
        if(i == c_program - 1)
          return result;
        else if(result && result->state & CTH_VARSTATE__FREEALL)
          cth_variable_destroy(result);
      }
      return NULL;
    case CTH_AST__BINARY:
    case CTH_AST__UNARY:
      return cth_ast__functions[node->op](node, variables, path, err);
    case CTH_AST__ID:
      return cth_op__name(node, variables, path, err);
    case CTH_AST__DECLARATION:
      return cth_op__declaration(node, variables, path, err);
    case CTH_AST__IF:
      return cth_op__if(node, variables, path, err);
    default:
      fprintf(stderr, "ERROR: option %d not currently implemented.\n", node->type);
      return NULL;
  }
}

#endif
