#ifndef LTH_INTERPRETER_H_INCLUDED
#define LTH_INTERPRETER_H_INCLUDED

#include "ctx.h"                    // interpreter ctx.
#include "ops/_ops.h"               // lth_interpreter_ops.
#include "../common/_common.h"      // array handling.

// stdlib includes.
#include "stddef.h"                 // size_t.
#include "stdbool.h"                // boolean types.

// dispatches op.
static void lth_interpreter_op(
  const lth__underlying_tokT op, const bool prev_was_operand,
  lth_interpreterT* const restrict interpreter,
  lth_error_ctxT* const restrict err, rcT* const restrict path, const lth_fposT line, const lth_fposT offset
){
  const lth_interpreter_opT foo = lth_interpreter_ops[op][prev_was_operand];
  if(!foo)
    lth_error_report(LTH_ERROR__INTERPRETER_OP_UNIMPLEMENTED, LTH_ERROR__COMPILER, path, line, offset, err);
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
static lth_outputT lth_interpreter_run(
  lth_tokT* const restrict code, const size_t c_code,
  lth_error_ctxT* const restrict error,
  lth_variable_ctxT* const restrict variables,
  const bool isrepl
){
  lth_interpreterT interpreter; lth_interpreter_init(&interpreter, variables);

  bool prev_was_operand = false;                                // used to check if two operands/names follow each other, which is not valid.
// c_code - 1 because last tok is always EOF.
  for(size_t pos = 0; pos < c_code - 1; pos++){                 // overusage of 'i' as index variable found harmful.
    if(code[pos].token & LTH_TOK__NAME_BIT){
      if(prev_was_operand){
        lth_error_report(LTH_ERROR__INTERPRETER_ADJACENT_OPERANDS, LTH_ERROR__COMPILER, NULL /* TODO! */, code[pos].line, code[pos].offset, error);
        return lth_interpreter_destroy(&interpreter);
      }
      else prev_was_operand = true;

      lth_interpreter_push_value(&interpreter, code[pos].token);
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
      lth__underlying_tokT top = interpreter.ops[cnt_top - 1];
      if(lth_tok__priority[top] <= lth_tok__priority[code[pos].token])
        goto SKIP_PRIO;
      vfastarr_remove(interpreter.ops, sizeof(*interpreter.ops));
      do{
        lth_interpreter_op(top, prev_was_operand, &interpreter, error, NULL /* TODO! */, code[pos].line, code[pos].offset);
        top = lth_interpreter_pop_op(&interpreter);
      } while(lth_tok__priority[top] > lth_tok__priority[code[pos].token]);
    }
SKIP_PRIO:
    prev_was_operand = false;
    if(code[pos].token != LTH_TOK__SEMICOLON)
      lth_interpreter_push_op(&interpreter, code[pos].token);
  }

  if(!isrepl)
    return lth_interpreter_destroy(&interpreter);
// result of execution is first few values at top of interpreter.
  lth_outputT result = interpreter.values;
  interpreter.values = NULL;
  interpreter.variables = NULL;

  vfastarr_destroy(lth_interpreter_destroy(&interpreter));
  return result;
}

#endif
