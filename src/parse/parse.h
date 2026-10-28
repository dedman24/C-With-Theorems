#ifndef CTH_PARSE_H_INCLUDED
#define CTH_PARSE_H_INCLUDED

// parses command-line options, returns struct holding all of them.
//    cth_opT    ~ controls what operation the compiler's executing.

#include "../common/err/err.h"

// stdlib includes.
#include "help.h"
#include "stddef.h"     // size_t, NULL.
#include "stdint.h"     // uint8_t.
#include "stdlib.h"     // calloc, free.
#include "string.h"     // strncmp.
#include "stdbool.h"    // bool, true, false.

#define CTH_DEFAULT_OPTION CTH_OPTIONS__DEBUG
#define CTH_DEFAULT_DST "program.out"

// note: most of these are left over from other projects.
typedef enum{
    CTH_OPTIONS__REPL = 3,          // Read-Evaluate-Print-Loop.
    CTH_OPTIONS__HELP = 1,          // prints help info.
    CTH_OPTIONS__CHECK = 2,         // checks for type validity of program.
    CTH_OPTIONS__DETOKENISE = 5,    // detokenises tokenised program.
    CTH_OPTIONS__TOKENISE = 0,      // tokenises program.
    CTH_OPTIONS__VERSION = 4,       // compiler version.

    CTH_OPTIONS__NONE = 6,
    CTH_OPTIONS__COMPILE,
    CTH_OPTIONS__DEFAULT = CTH_OPTIONS__COMPILE
} cth_opT;

// hash function, generated with vfasttables version 1.0.0.
static cth_opT cth_op__hash(const char* const restrict str, const size_t len){
    if(len > 127) return 0;
// associated values generated for hash function.
    const uint8_t associated_values[] = {
        141,149,34,18,48,72,80,155,55,9,101,243,237,161,49,229,
        98,87,76,184,213,242,97,102,177,203,44,247,35,132,82,64,
        175,84,73,190,112,225,47,197,95,238,7,95,44,236,253,243,
        211,4,249,241,224,43,151,122,57,85,18,58,108,160,92,244,
        57,36,43,149,126,136,55,193,99,206,163,240,131,161,75,92,
        0,234,50,212,71,219,208,156,41,26,196,240,6,180,40,242,
        208,19,90,20,18,175,151,100,103,68,117,17,228,95,136,34,
        189,202,165,13,225,66,237,195,112,37,200,66,248,28,13,93
    };
    const size_t indices[] = {
        0 
    };
    uint8_t hash = associated_values[len];
    for(size_t i = 0; i < sizeof(indices)/sizeof(*indices); i++){
        hash += associated_values[str[indices[i] % len]];
    }
    hash %= 6;
    return hash;
}

static cth_opT cth_op__search(const char* const restrict str, const size_t len){
    const cth_opT index = cth_op__hash(str, len);
    char* table[6] = {
       [CTH_OPTIONS__REPL] = "repl",
       [CTH_OPTIONS__TOKENISE] = "tokenise",
       [CTH_OPTIONS__VERSION] = "version",
       [CTH_OPTIONS__DETOKENISE] = "detokenise",
       [CTH_OPTIONS__HELP] = "help",
       [CTH_OPTIONS__CHECK] = "check"
    };
    return strncmp(table[index], str, len) == 0? index: CTH_OPTIONS__NONE;
}

typedef struct{
  const char* restrict path_src;
  cth_opT op;
} cth_optionsT;

static void cth_options_destroy(cth_optionsT* const restrict options, const bool freectx){
  free(options);
}

static cth_optionsT* cth_parse(
  cth_optionsT* restrict options,
  const int argc, 
  const char* const restrict* const restrict argv,
  cth_error_ctxT* const restrict error
){
  const bool dynamically_allocated = options == NULL;
  if(dynamically_allocated) options = malloc(sizeof(*options));
  options->path_src = NULL;
// ./cth, instead of erroring out, runs REPLh.
  if(argc == 1){
    options->op = CTH_OPTIONS__REPL;
    return options;
  }
  options->op = CTH_OPTIONS__NONE;

  for(size_t i = 1; i < argc; i++){
    const cth_opT op = cth_op__search(argv[i], strlen(argv[i]));

    if(op == CTH_OPTIONS__NONE) 
      cth_error_report__str(CTH_ERROR__PARSE_UNKNOWN_OPTION, CTH_ERROR__PARSE, NULL, 0, 0, error, argv[i]);
    else if(op == CTH_OPTIONS__HELP){
      cth_parse__help(argv[0]);
      cth_options_destroy(options, dynamically_allocated);
      return NULL;
    }
    else if(op == CTH_OPTIONS__VERSION){
      cth_parse__version();
      cth_options_destroy(options, dynamically_allocated);
      return NULL;
    }
// list of all valid compiler modes/states.
    else if(
      op == CTH_OPTIONS__REPL ||
      op == CTH_OPTIONS__CHECK ||
      op == CTH_OPTIONS__TOKENISE ||
      op == CTH_OPTIONS__DETOKENISE
    ){
      if(options->op != CTH_OPTIONS__NONE){
        cth_error_report__str(CTH_ERROR__PARSE_REPEATED_OPTION, CTH_ERROR__PARSE, NULL, 0, 0, error, argv[i]);
        continue;
      }
      if(op != CTH_OPTIONS__REPL){
        if(i == argc - 1) break;
        options->path_src = argv[++i];
      }
      options->op = op;
    }
  }
// default values.
  if(options->op == CTH_OPTIONS__NONE) options->op = CTH_OPTIONS__DEFAULT;
  if(options->path_src == NULL && options->op != CTH_OPTIONS__REPL){ 
    cth_error_report(CTH_ERROR__PARSE_MISSING_SOURCE_FILE, CTH_ERROR__PARSE, NULL, 0, 0, error);
    cth_options_destroy(options, dynamically_allocated);
    return NULL;
  }

  return options;
}

#endif
