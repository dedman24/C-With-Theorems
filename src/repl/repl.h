#ifndef CTH_REPL_H_INCLUDED
#define CTH_REPL_H_INCLUDED

// this file handles repl stuff.
// we want an interface that evaluates whatever's written to stdin terminated by a semicolon & executes it.
// this means that '2+3' does nothing, while '2+3;' prints out 5, or of whatever arithmetic expression is in stdin.
// also supports piping, like with python3; `echo "whatever;" | ./cth repl` has the same effect as writing `whatever;` by hand.

#include "../parse/version.h"

#include "../common/_common.h"
#include "../common/ansi_colours.h"       // all manners of colours.

#include "../tokeniser/tokeniser.h"       // tokeniser stuff.
#include "../interpreter/interpreter.h"   // interpreterT.

// stdlib includes.
#include "stdio.h"              // fgetc, stdin, stderr.

// posix stuff.
#include "unistd.h"             // isatty.

static void cth_repl__result(cth_variableT** const restrict variables){
  const size_t c_vars = vfastarr_elcnt(variables, sizeof(*variables));
  size_t nonnull = 0;

  for(size_t i = 0; i < c_vars; i++){
    cth_variableT* const var = variables[i];
    if(var == NULL){
      fprintf(stdout, "NULL :: NULL");
      if(i != c_vars - 1)
        fputs(", ", stdout);
      continue;
    }
    else nonnull++;
    switch(var->type){
      case CTH_TYPE__U8:  fprintf(stdout, "%u",  *(uint8_t*)var->internal); break;
      case CTH_TYPE__U16: fprintf(stdout, "%u",  *(uint16_t*)var->internal); break;
      case CTH_TYPE__U32: fprintf(stdout, "%u",  *(uint32_t*)var->internal); break;
      case CTH_TYPE__U64: fprintf(stdout, "%lu", *(uint64_t*)var->internal); break;

      case CTH_TYPE__I8:  fprintf(stdout, "%d",  *(int8_t*)var->internal); break;
      case CTH_TYPE__I16: fprintf(stdout, "%d",  *(int16_t*)var->internal); break;
      case CTH_TYPE__I32: fprintf(stdout, "%d",  *(int32_t*)var->internal); break;
      case CTH_TYPE__I64: fprintf(stdout, "%ld", *(int64_t*)var->internal); break;

      case CTH_TYPE__F32: fprintf(stdout, "%f", *(float*)var->internal); break;
      case CTH_TYPE__F64: fprintf(stdout, "%f", *(double*)var->internal); break;

      default:
        fputs("sorry, output of this type isn't implemented yet :PPP.\n", stderr);
    }
    fputs(" :: ", stdout);
    fputs(cth_type__strings[var->type], stdout);

    if(i != c_vars - 1)
      fputs(", ", stdout);

    if(var->qualifier & CTH_QUALIFIER__FREEALL)
      cth_variable_destroy(var);
  }
  if(nonnull)
    fputc('\n', stdout);
}

static void cth_repl__line(const char* const restrict code, cth_tokeniserT* const restrict tokeniser){
  const size_t s_code = vfastarr_size(code);
  if(!s_code) return;

  cth_error_ctxT err; cth_error_ctx_init(&err);
  cth_outputT tokenised = cth_tokenise__repl(code, "repl", tokeniser, &err);

  cth_variableT** const restrict output = cth_interpret(tokenised, tokeniser->name.variables, NULL, &err);
  cth_repl__result(output);

  vfastarr_destroy(output);
  cth_ast_destroy(tokenised);
  cth_error_print(&err, false);
}

// loop that evaluates everything coming from stdin.
static cth_outputT cth_repl(void){
// to detect whether piping from stdout of another process is used, or this is just a normal user interface.
  const bool ishuman = isatty(STDIN_FILENO);
  if(ishuman){
    cth_parse__version();
    fwrite("> ", 2, 1, stdout);
    fflush(stdout);
  }

  cth_tokeniserT tokeniser; cth_tokeniser_init(&tokeniser);

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
      cth_repl__line(line, &tokeniser);
      vfastarr_empty(line);
    }
  }
// for some reason doesn't work with this, figure it out smartass.
/*
  for(size_t i = 0; i < vfastarr_size(line); i++)
    printf("%02x", line[i]);
  puts("");

  if(!ishuman){
    vfastarr_append(line, ";", sizeof(";"));      // appends ';\0'. TODO: enable.
    cth_repl__line(line);
  }
*/
  cth_astT* const restrict program = cth_tokeniser_destroy(&tokeniser, NULL, NULL, NULL);
  cth_ast_destroy(program);
  vfastarr_destroy(line);
  return NULL;
}

#endif
