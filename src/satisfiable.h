#ifndef SATISFIABLE_H_INCLUDED
#define SATISFIABLE_H_INCLUDED

// 'satisfiable' is a programming language that compiles directly to SAT clauses.

#include "common/err/err.h"
#include "common/output.h"
#include "common/variables.h"

satisfiable_outputT satisfiable_compile(const int argc, const char* const restrict argv[]);

#endif

#define SATISFIABLE_IMPLEMENTATION
#ifdef SATISFIABLE_IMPLEMENTATION

#include "repl/repl.h"
#include "parse/parse.h"
#include "common/_common.h"
#include "tokeniser/tokeniser.h"
#include "tokeniser/detokeniser.h"

satisfiable_outputT satisfiable_compile(const int argc, const char* const restrict argv[]){
  satisfiable_error_ctxT* const restrict err = satisfiable_error_ctx_init(NULL);
  satisfiable_optionsT* const restrict options = satisfiable_parse(NULL, argc, argv, err);
  if(!options){
    satisfiable_error_print(err, true);
    return NULL;
  }

  satisfiable__fileT file;
  if(options->op != SATISFIABLE_OPTIONS__REPL){
    file = satisfiable_load__map(options->path_src, false);
    if(!file.code){
      satisfiable_error_report__str(SATISFIABLE_ERROR__PARSE_MISSING_SOURCE_FILE, SATISFIABLE_ERROR__PARSE, NULL, 0, 0, err, options->path_src);
      satisfiable_error_print(err, true);
      satisfiable_options_destroy(options, true);
      return NULL;
    }
  }

  satisfiable_outputT output;
  switch(options->op){
    case SATISFIABLE_OPTIONS__DETOKENISE:
      output = satisfiable_detokenise((satisfiable_tokT*)file.code, file.size/sizeof(satisfiable_tokT));
      break;
    case SATISFIABLE_OPTIONS__CHECK:
    case SATISFIABLE_OPTIONS__COMPILE:
    case SATISFIABLE_OPTIONS__TOKENISE:
      satisfiable_variable_ctxT* variables;
      output = satisfiable_tokenise(file.code, options->path_src, strlen(options->path_src), &variables, err);
      satisfiable_variable_ctx_destroy(variables, true);
      break;
    case SATISFIABLE_OPTIONS__REPL:
      output = satisfiable_repl();
      break;
    default:
      satisfiable_error_report__u32(SATISFIABLE_ERROR__IMPOSSIBILITY, SATISFIABLE_ERROR__PARSE, NULL, 0, 0, err, SATISFIABLE_IMPOSSIBILITY__PARSE_OPTION);
      satisfiable_error_print(err, true);
      satisfiable_options_destroy(options, true);
      return NULL;
  }

  satisfiable_load__unmap(file);

  satisfiable_options_destroy(options, true);
  satisfiable_error_ctx_destroy(err, true);

  return output;
}

#endif
