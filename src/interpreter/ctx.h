#ifndef LTH_INTERPRETER_CTX_H_INCLUDED
#define LTH_INTERPRETER_CTX_H_INCLUDED

#include "../common/_common.h"

// stdlib includes.
#include "stddef.h"                 // size_t.

typedef struct{
  lth_variable_ctxT* variables;
  lth_nameT* state;
  lth_outputT output;
} lth_interpreterT;

static void lth_interpreter_init(
  lth_interpreterT* const restrict interpreter,
  lth_variable_ctxT* const restrict variables
){
  interpreter->variables = variables;

  interpreter->state = vfastarr_init(sizeof(*interpreter->state));
  interpreter->output = vfastarr_init(sizeof(char));
}

static lth_outputT lth_interpreter_destroy(lth_interpreterT* const restrict interpreter){
  if(interpreter->variables)
    lth_variable_ctx_destroy(interpreter->variables, true);
  if(interpreter->state)
    vfastarr_destroy(interpreter->state);
  return interpreter->output;
}

static void lth_interpreter_push(lth_interpreterT* const restrict interpreter, const lth_nameT value){
  interpreter->state = vfastarr_append(interpreter->state, &value, sizeof(value));
}

static lth_nameT lth_interpreter_pop(lth_interpreterT* const restrict interpreter){
  lth_nameT value;
  vfastarr_pop(interpreter->state, &value, sizeof(value));
  return value;
}

static void lth_interpreter_push_variable(
  lth_interpreterT* const restrict interpreter,
  void* const restrict internal, const lth_nameT type, const lth_qualifierT qualifier
){
  const lth_nameT id = lth_variable_name(interpreter->variables);
  lth_variable_add(interpreter->variables, id, NULL, 0, internal, type, qualifier);
  lth_interpreter_push(interpreter, id);
}

static lth_variableT* lth_interpreter_pop_variable(
  lth_interpreterT* const restrict interpreter,
  lth_error_ctxT* const restrict err, rcT* const restrict path, const lth_fposT line, const lth_fposT offset
){
  lth_nameT id;
  if(!vfastarr_pop(interpreter->state, &id, sizeof(id))){
    lth_error_report(LTH_ERROR__INTERPRETER_TOO_FEW_OPERANDS, LTH_ERROR__COMPILER, path, line, offset, err);
    return NULL;
  }

  lth_variableT* const restrict variable = lth_variable_search(interpreter->variables, id);
  if(!variable)
    lth_error_report(LTH_ERROR__INTERPRETER_VARIABLE_MISSING, LTH_ERROR__COMPILER, path, line, offset, err);
  return variable;
}

#endif
