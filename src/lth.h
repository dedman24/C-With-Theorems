#ifndef LTH_H_INCLUDED
#define LTH_H_INCLUDED

// 'LISP with theorems' is a dependent-typed version of LISP.

#include "common/err/err.h"
#include "common/output.h"
#include "common/variables.h"

lth_outputT lth_compile(const int argc, const char* const restrict argv[]);

#endif

#define LTH_IMPLEMENTATION
#ifdef LTH_IMPLEMENTATION

#include "repl/repl.h"
#include "parse/parse.h"
#include "common/_common.h"
#include "tokeniser/tokeniser.h"
#include "tokeniser/detokeniser.h"

lth_outputT lth_compile(const int argc, const char* const restrict argv[]){
  lth_error_ctxT* const restrict err = lth_error_ctx_init(NULL);
  lth_optionsT* const restrict options = lth_parse(NULL, argc, argv, err);
  if(!options){
    lth_error_print(err, true);
    return NULL;
  }

  lth__fileT file;
  if(options->op != LTH_OPTIONS__REPL){
    file = lth_load__map(options->path_src, false);
    if(!file.code){
      lth_error_report__str(LTH_ERROR__PARSE_MISSING_SOURCE_FILE, LTH_ERROR__PARSE, NULL, 0, 0, err, options->path_src);
      lth_error_print(err, true);
      lth_options_destroy(options, true);
      return NULL;
    }
  }

  lth_outputT output;
  switch(options->op){
    case LTH_OPTIONS__DETOKENISE:
      output = lth_detokenise((lth_tokT*)file.code, file.size/sizeof(lth_tokT));
      break;
    case LTH_OPTIONS__CHECK:
    case LTH_OPTIONS__COMPILE:
    case LTH_OPTIONS__TOKENISE:
      lth_variable_ctxT* variables;
      output = lth_tokenise(file.code, options->path_src, strlen(options->path_src), &variables, err);
      lth_variable_ctx_destroy(variables, true);
      break;
    case LTH_OPTIONS__REPL:
      output = lth_repl();
      break;
    default:
      lth_error_report__u32(LTH_ERROR__IMPOSSIBILITY, LTH_ERROR__PARSE, NULL, 0, 0, err, LTH_IMPOSSIBILITY__PARSE_OPTION);
      lth_error_print(err, true);
      lth_options_destroy(options, true);
      return NULL;
  }

  lth_load__unmap(file);

  lth_options_destroy(options, true);
  lth_error_ctx_destroy(err, true);

  return output;
}

#endif
