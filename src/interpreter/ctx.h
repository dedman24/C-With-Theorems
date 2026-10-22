#ifndef LTH_INTERPRETER_CTX_H_INCLUDED
#define LTH_INTERPRETER_CTX_H_INCLUDED

#include "../common/_common.h"

// stdlib includes.
#include "stddef.h"                 // size_t.

typedef struct{
  lth_variable_ctxT* variables;
  lth__underlying_tokT* ops;
  lth_nameT* values;
  lth_outputT output;
} lth_interpreterT;

static void lth_interpreter_init(
  lth_interpreterT* const restrict interpreter,
  lth_variable_ctxT* const restrict variables
){
  interpreter->variables = variables;

  interpreter->ops = vfastarr_init(sizeof(*interpreter->ops));
  interpreter->values = vfastarr_init(sizeof(*interpreter->values));
  interpreter->output = vfastarr_init(sizeof(char));
}

static lth_outputT lth_interpreter_destroy(lth_interpreterT* const restrict interpreter){
  if(interpreter->variables)
    lth_variable_ctx_destroy(interpreter->variables, true);

  if(interpreter->ops)
    vfastarr_destroy(interpreter->ops);
  if(interpreter->values)
    vfastarr_destroy(interpreter->values);

  return interpreter->output;
}

static void lth_interpreter_push_op(lth_interpreterT* const restrict interpreter, const lth__underlying_tokT op){
  interpreter->ops = vfastarr_append(interpreter->ops, &op, sizeof(op));
}

static lth__underlying_tokT lth_interpreter_pop_op(lth_interpreterT* const restrict interpreter){
  lth__underlying_tokT tok; vfastarr_pop(interpreter->ops, &tok, sizeof(tok));
  return tok;
}

static lth__underlying_tokT lth_interpreter_top_op(lth_interpreterT* const restrict interpreter){
  return interpreter->ops[vfastarr_elcnt(interpreter->ops, sizeof(*interpreter->ops)) - 1];
}

static void lth_interpreter_push_variable(
  lth_interpreterT* const restrict interpreter,
  const char* const restrict name, const size_t s_name,
  void* const restrict internal, const lth_typeT type, const lth_qualifierT qualifier
){
  const lth_nameT id = lth_variable_name(interpreter->variables);
  lth_variable_add(interpreter->variables, id, name, s_name, internal, type, qualifier);

  interpreter->values = vfastarr_append(interpreter->values, &id, sizeof(id));
}

static lth_variableT* lth_interpreter_pop_variable(
  lth_interpreterT* const restrict interpreter,
  lth_error_ctxT* const restrict err, rcT* const restrict path, const lth_fposT line, const lth_fposT offset
){
  lth_nameT id;
  if(!vfastarr_pop(interpreter->values, &id, sizeof(id))){
    lth_error_report(LTH_ERROR__INTERPRETER_TOO_FEW_OPERANDS, LTH_ERROR__COMPILER, path, line, offset, err);
    return NULL;
  }

  lth_variableT* const restrict variable = lth_variable_search(interpreter->variables, id);
  if(!variable)
    lth_error_report(LTH_ERROR__INTERPRETER_VARIABLE_MISSING, LTH_ERROR__COMPILER, path, line, offset, err);
  return variable;
}

static void lth_interpreter_push_value(lth_interpreterT* const restrict interpreter, lth_nameT value){
  value &= ~LTH_TOK__NAME_BIT;
  interpreter->values = vfastarr_append(interpreter->values, &value, sizeof(value));
}

static size_t lth_interpreter_depth_op(lth_interpreterT* const restrict interpreter){
  return vfastarr_elcnt(interpreter->ops, sizeof(*interpreter->ops));
}

#endif
