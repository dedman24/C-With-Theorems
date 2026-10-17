#ifndef SATISFIABLE_INTERPRETER_CTX_H_INCLUDED
#define SATISFIABLE_INTERPRETER_CTX_H_INCLUDED

#include "../common/_common.h"

// stdlib includes.
#include "stddef.h"                 // size_t.

typedef struct{
  satisfiable_variable_ctxT* variables;
  satisfiable__underlying_tokT* ops;
  satisfiable_nameT* values;
  satisfiable_outputT output;
} satisfiable_interpreterT;

static void satisfiable_interpreter_init(
  satisfiable_interpreterT* const restrict interpreter,
  satisfiable_variable_ctxT* const restrict variables
){
  interpreter->variables = variables;

  interpreter->ops = vfastarr_init(sizeof(*interpreter->ops));
  interpreter->values = vfastarr_init(sizeof(*interpreter->values));
  interpreter->output = vfastarr_init(sizeof(char));
}

static satisfiable_outputT satisfiable_interpreter_destroy(satisfiable_interpreterT* const restrict interpreter){
  if(interpreter->variables)
    satisfiable_variable_ctx_destroy(interpreter->variables, true);

  if(interpreter->ops)
    vfastarr_destroy(interpreter->ops);
  if(interpreter->values)
    vfastarr_destroy(interpreter->values);

  return interpreter->output;
}

static void satisfiable_interpreter_push_op(satisfiable_interpreterT* const restrict interpreter, const satisfiable__underlying_tokT op){
  interpreter->ops = vfastarr_append(interpreter->ops, &op, sizeof(op));
}

static satisfiable__underlying_tokT satisfiable_interpreter_pop_op(satisfiable_interpreterT* const restrict interpreter){
  satisfiable__underlying_tokT tok; vfastarr_pop(&interpreter->ops, &tok, sizeof(tok));
  return tok;
}

static satisfiable__underlying_tokT satisfiable_interpreter_top_op(satisfiable_interpreterT* const restrict interpreter){
  return interpreter->ops[vfastarr_elcnt(interpreter->ops, sizeof(*interpreter->ops)) - 1];
}

static void satisfiable_interpreter_push_variable(
  satisfiable_interpreterT* const restrict interpreter,
  const char* const restrict name, const size_t s_name,
  void* const restrict internal, const satisfiable_typeT type, const satisfiable_qualifierT qualifier
){
  const satisfiable_nameT id = satisfiable_variable_name(interpreter->variables);
  satisfiable_variable_add(interpreter->variables, id, name, s_name, internal, type, qualifier);

  interpreter->values = vfastarr_append(interpreter->values, &id, sizeof(id));
}

static satisfiable_variableT* satisfiable_interpreter_pop_variable(
  satisfiable_interpreterT* const restrict interpreter,
  satisfiable_error_ctxT* const restrict err, rcT* const restrict path, const satisfiable_fposT line, const satisfiable_fposT offset
){
  satisfiable_nameT tok;
  if(!vfastarr_pop(&interpreter->values, &tok, sizeof(tok))){
    satisfiable_error_report(SATISFIABLE_ERROR__INTERPRETER_TOO_FEW_OPERANDS, SATISFIABLE_ERROR__COMPILER, path, line, offset, err);
    return NULL;
  }

  satisfiable_variableT* const restrict variable = satisfiable_variable_search(interpreter->variables, tok);
  if(!variable)
    satisfiable_error_report(SATISFIABLE_ERROR__INTERPRETER_VARIABLE_MISSING, SATISFIABLE_ERROR__COMPILER, path, line, offset, err);
  return variable;
}

static void satisfiable_interpreter_push_value(satisfiable_interpreterT* const restrict interpreter, satisfiable_nameT value){
  value &= ~SATISFIABLE_TOK__NAME_BIT;
  interpreter->ops = vfastarr_append(interpreter->values, &value, sizeof(value));
}

static size_t satisfiable_interpreter_depth_op(satisfiable_interpreterT* const restrict interpreter){
  return vfastarr_elcnt(interpreter->ops, sizeof(*interpreter->ops));
}

#endif
