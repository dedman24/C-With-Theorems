#ifndef SATISFIABLE_PARSE_H_INCLUDED
#define SATISFIABLE_PARSE_H_INCLUDED

// parses command-line options, returns struct holding all of them.
//    satisfiable_opT    ~ controls what operation the compiler's executing.

#include "../common/err/err.h"

// stdlib includes.
#include "help.h"
#include "stddef.h"     // size_t, NULL.
#include "stdint.h"     // uint8_t.
#include "stdlib.h"     // calloc, free.
#include "string.h"     // strncmp.
#include "stdbool.h"    // bool, true, false.

#define SATISFIABLE_DEFAULT_OPTION SATISFIABLE_OPTIONS__DEBUG
#define SATISFIABLE_DEFAULT_DST "program.out"

// note: most of these are left over from other projects.
typedef enum{
    SATISFIABLE_OPTIONS__REPL = 3,          // Read-Evaluate-Print-Loop in Satisfiable.
    SATISFIABLE_OPTIONS__HELP = 1,          // prints help info.
    SATISFIABLE_OPTIONS__CHECK = 2,         // checks for type validity of program.
    SATISFIABLE_OPTIONS__DETOKENISE = 5,    // detokenises tokenised program.
    SATISFIABLE_OPTIONS__TOKENISE = 0,      // tokenises program.
    SATISFIABLE_OPTIONS__VERSION = 4,       // compiler version.

    SATISFIABLE_OPTIONS__NONE = 6,
    SATISFIABLE_OPTIONS__COMPILE,
    SATISFIABLE_OPTIONS__DEFAULT = SATISFIABLE_OPTIONS__COMPILE
} satisfiable_opT;

// hash function, generated with vfasttables version 1.0.0.
static satisfiable_opT satisfiable_op__hash(const char* const restrict str, const size_t len){
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

static satisfiable_opT satisfiable_op__search(const char* const restrict str, const size_t len){
    const satisfiable_opT index = satisfiable_op__hash(str, len);
    char* table[6] = {
       [SATISFIABLE_OPTIONS__REPL] = "repl",
       [SATISFIABLE_OPTIONS__TOKENISE] = "tokenise",
       [SATISFIABLE_OPTIONS__VERSION] = "version",
       [SATISFIABLE_OPTIONS__DETOKENISE] = "detokenise",
       [SATISFIABLE_OPTIONS__HELP] = "help",
       [SATISFIABLE_OPTIONS__CHECK] = "check"
    };
    return strncmp(table[index], str, len) == 0? index: SATISFIABLE_OPTIONS__NONE;
}

typedef struct{
  const char* restrict path_src;
  satisfiable_opT op;
} satisfiable_optionsT;

static void satisfiable_options_destroy(satisfiable_optionsT* const restrict options, const bool freectx){
  free(options);
}

static satisfiable_optionsT* satisfiable_parse(
  satisfiable_optionsT* restrict options,
  const int argc, 
  const char* const restrict* const restrict argv,
  satisfiable_error_ctxT* const restrict error
){
  const bool dynamically_allocated = options == NULL;
  if(dynamically_allocated) options = malloc(sizeof(*options));
  options->op = SATISFIABLE_OPTIONS__NONE;
  options->path_src = NULL;

  for(size_t i = 1; i < argc; i++){
    const satisfiable_opT op = satisfiable_op__search(argv[i], strlen(argv[i]));

    if(op == SATISFIABLE_OPTIONS__NONE) 
      satisfiable_error_report__str(SATISFIABLE_ERROR__PARSE_UNKNOWN_OPTION, SATISFIABLE_ERROR__PARSE, NULL, 0, 0, error, argv[i]);
    else if(op == SATISFIABLE_OPTIONS__HELP){
      satisfiable_parse__help(argv[0]);
      satisfiable_options_destroy(options, dynamically_allocated);
      return NULL;
    }
    else if(op == SATISFIABLE_OPTIONS__VERSION){
      satisfiable_parse__version();
      satisfiable_options_destroy(options, dynamically_allocated);
      return NULL;
    }
// list of all valid compiler modes/states.
    else if(
      op == SATISFIABLE_OPTIONS__REPL ||
      op == SATISFIABLE_OPTIONS__CHECK ||
      op == SATISFIABLE_OPTIONS__TOKENISE ||
      op == SATISFIABLE_OPTIONS__DETOKENISE
    ){
      if(options->op != SATISFIABLE_OPTIONS__NONE){
        satisfiable_error_report__str(SATISFIABLE_ERROR__PARSE_REPEATED_OPTION, SATISFIABLE_ERROR__PARSE, NULL, 0, 0, error, argv[i]);
        continue;
      }
      if(op != SATISFIABLE_OPTIONS__REPL){
        if(i == argc - 1) break;
        options->path_src = argv[++i];
      }
      options->op = op;
    }
  }
// default values.
  if(options->op == SATISFIABLE_OPTIONS__NONE) options->op = SATISFIABLE_OPTIONS__DEFAULT;
  if(options->path_src == NULL && options->op != SATISFIABLE_OPTIONS__REPL){ 
    satisfiable_error_report(SATISFIABLE_ERROR__PARSE_MISSING_SOURCE_FILE, SATISFIABLE_ERROR__PARSE, NULL, 0, 0, error);
    satisfiable_options_destroy(options, dynamically_allocated);
    return NULL;
  }

  return options;
}

#endif
