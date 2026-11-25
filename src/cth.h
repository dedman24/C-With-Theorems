#ifndef CTH_H_INCLUDED
#define CTH_H_INCLUDED

// 'C With Theorems' is a dependent-typed version of the C programming language.

#include "common/output.h"
#include "common/err/err.h"
#include "common/variables/variables.h"

// returns true on failure, false otherwise.
bool cth_compile(const int argc, const char* const restrict argv[]);

#endif

#define CTH_IMPLEMENTATION
#ifdef CTH_IMPLEMENTATION

#include "repl/repl.h"
#include "parse/parse.h"
#include "common/_common.h"
#include "tokeniser/tokeniser.h"

bool cth_compile(const int argc, const char* const restrict argv[]){
  cth_error_ctxT* const restrict err = cth_error_ctx_init(NULL);
  cth_optionsT* const restrict options = cth_parse(NULL, argc, argv, err);
  if(!options){
    cth_error_print(err, true);
    return false;
  }

  cth__fileT file;
  if(options->op != CTH_OPTIONS__REPL && options->op != CTH_OPTIONS__REPL_STR){
    file = cth_load__map(options->path_src, false);
    if(!file.code){
      cth_error_report__str(CTH_ERROR__PARSE_MISSING_SOURCE_FILE, CTH_ERROR__PARSE, NULL, 0, 0, err, options->path_src);
      cth_error_print(err, true);
      cth_options_destroy(options, true);
      return false;
    }
  }

  bool success = true;
  switch(options->op){
    case CTH_OPTIONS__DETOKENISE:    
      fputs("'cth detokeniser' has been deprecated; perhaps it will be added in the future.\n", stderr);
      success = false;
      break;
    case CTH_OPTIONS__CHECK:
    case CTH_OPTIONS__COMPILE:
    case CTH_OPTIONS__TOKENISE:
      cth_variable_ctxT* variables;
      success = cth_tokenise(file.code, options->path_src, &variables, err);
      cth_variable_ctx_destroy(variables, true);
      break;
    case CTH_OPTIONS__REPL:
      cth_repl();
      break;
    case CTH_OPTIONS__REPL_STR:
      success = cth_repl_string(argv[argc-1]);
      break;
    default:
      cth_error_report__u32(CTH_ERROR__IMPOSSIBILITY, CTH_ERROR__PARSE, NULL, 0, 0, err, CTH_IMPOSSIBILITY__PARSE_OPTION);
      cth_error_print(err, true);
      cth_options_destroy(options, true);
      success = false;
  }

  cth_load__unmap(file);

  cth_options_destroy(options, true);
  cth_error_ctx_destroy(err, true);

  return success;
}

#endif
