#ifndef SATISFIABLE_INTERPRETER_H_INCLUDED
#define SATISFIABLE_INTERPRETER_H_INCLUDED

#include "ctx.h"                    // interpreter ctx.
#include "ops/_ops.h"               // satisfiable_interpreter_ops.
#include "../common/_common.h"      // array handling.

// stdlib includes.
#include "stddef.h"                 // size_t.
#include "stdbool.h"                // boolean types.

// dispatches op.
static void satisfiable_interpreter_op(
  const satisfiable__underlying_tokT op, const bool prev_was_operand,
  satisfiable_interpreterT* const restrict interpreter,
  satisfiable_error_ctxT* const restrict err, rcT* const restrict path, const satisfiable_fposT line, const satisfiable_fposT offset
){
  const satisfiable_interpreter_opT foo = satisfiable_interpreter_ops[op][prev_was_operand];
  if(!foo)
    satisfiable_error_report(SATISFIABLE_ERROR__INTERPRETER_OP_UNIMPLEMENTED, SATISFIABLE_ERROR__COMPILER, path, line, offset, err);
  else foo(interpreter, err, path, line, offset);
}

// this function interprets bytecode directly.
// it has two stacks:
//  one for operands,
//  one for values.
// each token has a 'priority' assigned to it.

// c_code ~ countof code.
// destroys 'variables'.
// isrepl ~ when set to false, interprets program normally.
//          when set to true, executes as REPL.
static satisfiable_outputT satisfiable_interpreter_run(
  satisfiable_tokT* const restrict code, const size_t c_code,
  satisfiable_error_ctxT* const restrict error,
  satisfiable_variable_ctxT* const restrict variables,
  const bool isrepl
){
  satisfiable_interpreterT interpreter; satisfiable_interpreter_init(&interpreter, variables);

  bool prev_was_operand = false;                                // used to check if two operands/names follow each other, which is not valid.
// c_code - 1 because last tok is always EOF.
  for(size_t pos = 0; pos < c_code - 1; pos++){                 // overusage of 'i' as index variable found harmful.
    if(code[pos].token & SATISFIABLE_TOK__NAME_BIT){
      if(prev_was_operand){
        satisfiable_error_report(SATISFIABLE_ERROR__INTERPRETER_ADJACENT_OPERANDS, SATISFIABLE_ERROR__COMPILER, NULL /* TODO! */, code[pos].line, code[pos].offset, error);
        return satisfiable_interpreter_destroy(&interpreter);
      }
      else prev_was_operand = true;

      satisfiable_interpreter_push_value(&interpreter, code[pos].token);
      continue;
    }

// the following tokens require special processing rules:
//  [X] semicolons.
//  [ ] ufcs stuff.
//  [ ] round brackets.
//  [ ] square brackets (in types too).
//  [ ] match statements.
//  [ ] functions.

// while priority(top) > priority(ourtok) process tokens.
    const size_t cnt_top = vfastarr_elcnt(interpreter.ops, sizeof(*interpreter.ops));

    if(cnt_top){
      satisfiable__underlying_tokT top = interpreter.ops[cnt_top - 1];
      if(satisfiable_tok__priority[top] <= satisfiable_tok__priority[code[pos].token])
        goto SKIP_PRIO;
      vfastarr_remove(interpreter.ops, sizeof(*interpreter.ops));
      do{
        satisfiable_interpreter_op(top, prev_was_operand, &interpreter, error, NULL /* TODO! */, code[pos].line, code[pos].offset);
        top = satisfiable_interpreter_pop_op(&interpreter);
      } while(satisfiable_tok__priority[top] > satisfiable_tok__priority[code[pos].token]);
    }
SKIP_PRIO:
    prev_was_operand = false;
    if(code[pos].token != SATISFIABLE_TOK__SEMICOLON)
      satisfiable_interpreter_push_op(&interpreter, code[pos].token);
  }

  if(!isrepl)
    return satisfiable_interpreter_destroy(&interpreter);
// result of execution is first few values at top of interpreter.
  satisfiable_outputT result = interpreter.values;
  interpreter.values = NULL;
  interpreter.variables = NULL;

  vfastarr_destroy(satisfiable_interpreter_destroy(&interpreter));
  return result;
}

#endif
