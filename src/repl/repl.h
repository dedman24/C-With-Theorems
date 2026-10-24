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

static void cth_repl__result(cth_nameT* const restrict name, cth_variable_ctxT* const restrict variables){
  const size_t c_name = vfastarr_elcnt(name, sizeof(*name));

  for(size_t i = 0; i < c_name; i++){
    cth_variableT* const restrict var = cth_variable_search(variables, name[i]);

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
    fputs(" : ", stdout);
    fputs(cth_type__strings[var->type], stdout);

    if(i != c_name - 1)
      fputs(", ", stdout);
  }
  fputc('\n', stdout);
}

static void cth_repl__line(const char* const restrict code){
  const size_t s_code = vfastarr_size(code);
  if(!s_code) return;

  cth_variable_ctxT* variables;
  cth_error_ctxT err; cth_error_ctx_init(&err);
  cth_outputT tokenised = cth_tokenise(code, "repl", &variables, &err);

  cth_nameT* const restrict name = NULL;
    /*was cth_interpreter_run(tokenised, vfastarr_elcnt(tokenised, sizeof(cth_tokT)), &err, variables, true);*/
  cth_repl__result(name, variables);

  vfastarr_destroy(name);
  vfastarr_destroy(tokenised);
  cth_error_print(&err, false);
  cth_variable_ctx_destroy(variables, true);
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
      cth_repl__line(line);
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
  vfastarr_destroy(line);
  return NULL;
}

#endif
