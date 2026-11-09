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
#include "inttypes.h"           // integer type output.

// posix stuff.
#include "unistd.h"             // isatty.

static void cth_repl__result(cth_variableT** const restrict variables, cth_variable_ctxT* const restrict ctx){
  const size_t c_vars = vfastarr_elcnt(variables, sizeof(*variables));

  for(size_t i = 0; i < c_vars; i++){
    cth_variableT* const var = variables[i];
    if(var == NULL){
      fprintf(stdout, "NULL :: NULL");
      if(i != c_vars - 1)
        fputs(", ", stdout);
      continue;
    }
    cth_variableT* const type_var = cth_variable_search(ctx, var->type);
    const struct cth_typecheck_top_lvl_getS top = cth_typecheck_top_lvl_get(type_var->internal.type->tree);
    switch(top.dispatch){
      case CTH_TYPE__U8:  fprintf(stdout, "%u",  var->internal.u8); break;
      case CTH_TYPE__U16: fprintf(stdout, "%u",  var->internal.u16); break;
      case CTH_TYPE__U32: fprintf(stdout, "%u",  var->internal.u32); break;
      case CTH_TYPE__U64: fprintf(stdout, "%lu", var->internal.u64); break;

      case CTH_TYPE__I8:  fprintf(stdout, "%d",  var->internal.i8); break;
      case CTH_TYPE__I16: fprintf(stdout, "%d",  var->internal.i16); break;
      case CTH_TYPE__I32: fprintf(stdout, "%d",  var->internal.i32); break;
      case CTH_TYPE__I64: fprintf(stdout, "%ld", var->internal.i64); break;

      case CTH_TYPE__F32: fprintf(stdout, "%f", var->internal.f32); break;
      case CTH_TYPE__F64: fprintf(stdout, "%f", var->internal.f64); break;

      case CTH_TYPE__BOOLEAN:
        if(var->internal.boolean)
          fputs(cth_variable_name__true, stdout);
        else
          fputs(cth_variable_name__false, stdout);
        break;
      case CTH_TYPE__TYPE:
        size_t s_str_type;
        char* const restrict str_type = cth_type_str_construct(var->internal.type->tree, ctx, &s_str_type);

        fputs(str_type, stdout);
        vfastarr_destroy(str_type);
        break;
      case CTH_QUALIFIER__PTR:
      case CTH_QUALIFIER__REF:
        if(var->internal.ptr)
          fprintf(stdout, "%" PRIuPTR, var->internal.ptr);
        else
          fputs("NULL", stdout);
        break;
      default:
        fputs("sorry, output of this type isn't implemented yet :PPP.\n", stderr);
    }
    fputs(" :: ", stdout);
    {
      size_t s_type_name;
      char* const restrict type_name = cth_type_str_construct(type_var->internal.type->tree, ctx, &s_type_name);
      printf("%.*s", (int)s_type_name, type_name);
      vfastarr_destroy(type_name);
    }

    if(i != c_vars - 1)
      fputs(", ", stdout);

    if(var->state & CTH_VARSTATE__FREEALL)
      cth_variable_destroy(var);
  }
  if(c_vars) fputc('\n', stdout);
}

static void cth_repl__line(const char* const restrict code, cth_tokeniserT* const restrict tokeniser){
  const size_t s_code = vfastarr_size(code);
  if(!s_code) return;

  cth_error_ctxT err; cth_error_ctx_init(&err);
  cth_outputT tokenised = cth_tokenise__repl(code, "repl", tokeniser, &err);

  cth_variableT** const restrict output = cth_interpret(tokenised, tokeniser->name.variables, NULL, &err);
  cth_repl__result(output, tokeniser->name.variables);

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
