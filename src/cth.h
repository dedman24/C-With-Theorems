#ifndef CTH_H_INCLUDED
#define CTH_H_INCLUDED

// 'LISP with theorems' is a dependent-typed version of LISP.

#include "common/err/err.h"
#include "common/output.h"
#include "common/variables.h"

cth_outputT cth_compile(const int argc, const char* const restrict argv[]);

#endif

#define CTH_IMPLEMENTATION
#ifdef CTH_IMPLEMENTATION

#include "repl/repl.h"
#include "parse/parse.h"
#include "common/_common.h"
#include "tokeniser/tokeniser.h"

cth_outputT cth_compile(const int argc, const char* const restrict argv[]){
  cth_error_ctxT* const restrict err = cth_error_ctx_init(NULL);
  cth_optionsT* const restrict options = cth_parse(NULL, argc, argv, err);
  if(!options){
    cth_error_print(err, true);
    return NULL;
  }

  cth__fileT file;
  if(options->op != CTH_OPTIONS__REPL){
    file = cth_load__map(options->path_src, false);
    if(!file.code){
      cth_error_report__str(CTH_ERROR__PARSE_MISSING_SOURCE_FILE, CTH_ERROR__PARSE, NULL, 0, 0, err, options->path_src);
      cth_error_print(err, true);
      cth_options_destroy(options, true);
      return NULL;
    }
  }

  cth_outputT output;
  switch(options->op){
    case CTH_OPTIONS__DETOKENISE:    
      fputs("'cth detokeniser' has been deprecated; perhaps it will be added in the future.\n", stderr);
      break;
    case CTH_OPTIONS__CHECK:
    case CTH_OPTIONS__COMPILE:
    case CTH_OPTIONS__TOKENISE:
      cth_variable_ctxT* variables;
      output = cth_tokenise(file.code, options->path_src, &variables, err);
      cth_variable_ctx_destroy(variables, true);
      break;
    case CTH_OPTIONS__REPL:
      output = cth_repl();
      break;
    default:
      cth_error_report__u32(CTH_ERROR__IMPOSSIBILITY, CTH_ERROR__PARSE, NULL, 0, 0, err, CTH_IMPOSSIBILITY__PARSE_OPTION);
      cth_error_print(err, true);
      cth_options_destroy(options, true);
      return NULL;
  }

  cth_load__unmap(file);

  cth_options_destroy(options, true);
  cth_error_ctx_destroy(err, true);

  return output;
}

#endif
