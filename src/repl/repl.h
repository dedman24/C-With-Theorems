#ifndef SATISFIABLE_REPL_H_INCLUDED
#define SATISFIABLE_REPL_H_INCLUDED

// this file handles repl stuff.
// we want an interface that evaluates whatever's written to stdin terminated by a semicolon & executes it.
// this means that '2+3' does nothing, while '2+3;' prints out 5, or of whatever arithmetic expression is in stdin.

#include "../common/_common.h"
#include "../common/ansi_colours.h"       // all manners of colours.

#include "../tokeniser/tokeniser.h"       // 
#include "../interpreter/interpreter.h"   // interpreterT.

// stdlib includes.
#include "stdio.h"              // fgetc, stdin, stderr.

// posix stuff.
#include "unistd.h"             // isatty.

static void satisfiable_repl_result(satisfiable_variableT* const restrict var){
  switch(var->type){
    case SATISFIABLE_TYPE__I32:
      fprintf(stdout, "%d", *(int32_t*)var->internal);
    default:
      fputs("sorry, output of this type isn't implemented yet :PPP.\n", stderr);
  }
}

static void satisfiable_repl__line(const char* const restrict code){
  const size_t s_code = vfastarr_size(code);

  satisfiable_variable_ctxT* variables;
  satisfiable_error_ctxT err; satisfiable_error_ctx_init(&err);
  satisfiable_outputT tokenised = satisfiable_tokenise(code, "repl", strlen("repl"), &variables, &err);

  satisfiable_nameT* const restrict name = satisfiable_interpreter_run(tokenised, vfastarr_elcnt(tokenised, sizeof(satisfiable_tokT)), &err, variables, true);
  const size_t c_name = vfastarr_elcnt(name, sizeof(*name));

  for(size_t i = 0; i < c_name; i++){
    satisfiable_variableT* const restrict var = satisfiable_variable_search(variables, name[i]);
    satisfiable_repl_result(var);
    if(i != c_name - 1)
      fputs(", ", stdout);
  }

  fputc('\n', stdout);

  vfastarr_destroy(name);
  satisfiable_variable_ctx_destroy(variables, true);
}

// loop that evaluates everything coming from stdin.
static satisfiable_outputT satisfiable_repl(void){
// to detect whether piping from stdout of another process is used, or this is just a normal user interface.
  const bool ishuman = isatty(STDIN_FILENO);
  if(ishuman){
    fputs(ANSI_COLOUR_MAGENTA "Satisfiable REPL" ANSI_COLOUR_RESET ".\n", stdout);
    fwrite("> ", 2, 1, stdout);
    fflush(stdout);
  }

  char* restrict line = vfastarr_init(sizeof(*line));
  while(1){
    const char c = fgetc(stdin);
    if(c == EOF){
      if(ishuman) continue;
      else break;
    }
    line = vfastarr_append(line, &c, sizeof(char));

    if(c == '\n' && ishuman){
      fwrite("> ", 2, 1, stdout);
      fflush(stdout);
    }
    else if(c == ';'){          // execute REPLed code, empty line.
      line = vfastarr_append(line, "\0", 1);
      satisfiable_repl__line(line);
      vfastarr_empty(line);
    }
  }
  if(!ishuman){
    vfastarr_append(line, ";", sizeof(";"));      // appends ';\0'.
    satisfiable_repl__line(line);
  }

  vfastarr_destroy(line);
  return NULL;
}

#endif
