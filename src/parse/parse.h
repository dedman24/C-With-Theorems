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
    SATISFIABLE_OPTIONS__HELP = 4,          // prints help info.
    SATISFIABLE_OPTIONS__CHECK = 0,         // checks for type validity of program.
    SATISFIABLE_OPTIONS__DETOKENISE = 2,    // detokenises tokenised program.
    SATISFIABLE_OPTIONS__TOKENISE = 1,      // tokenises program.
    SATISFIABLE_OPTIONS__VERSION = 3,       // compiler version.

    SATISFIABLE_OPTIONS__NONE = 5,
    SATISFIABLE_OPTIONS__COMPILE,
    SATISFIABLE_OPTIONS__DEFAULT = SATISFIABLE_OPTIONS__COMPILE
} satisfiable_opT;

// hash function, generated with vfasttables version 1.0.0.
static satisfiable_opT satisfiable_op__hash(const char* const restrict str, const size_t len){
    if(len > 10) return 0;
// associated values generated through hash function.
    const uint8_t associated_values[] = {
        0,0,0,0,4,0,0,3,1,0,2
    };
    return associated_values[len] % 5;
}

static satisfiable_opT satisfiable_op__search(const char* const restrict str, const size_t len){
    const satisfiable_opT index = satisfiable_op__hash(str, len);
    char* table[5] = {
       [SATISFIABLE_OPTIONS__HELP] = "help",
       [SATISFIABLE_OPTIONS__CHECK] = "check",
       [SATISFIABLE_OPTIONS__DETOKENISE] = "detokenise",
       [SATISFIABLE_OPTIONS__TOKENISE] = "tokenise",
       [SATISFIABLE_OPTIONS__VERSION] = "version"
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
      op == SATISFIABLE_OPTIONS__CHECK ||
      op == SATISFIABLE_OPTIONS__TOKENISE ||
      op == SATISFIABLE_OPTIONS__DETOKENISE
    ){
      if(options->op != SATISFIABLE_OPTIONS__NONE){
        satisfiable_error_report__str(SATISFIABLE_ERROR__PARSE_REPEATED_OPTION, SATISFIABLE_ERROR__PARSE, NULL, 0, 0, error, argv[i]);
        continue;
      }
      if(i == argc - 1)
        break;
      options->op = op;
      options->path_src = argv[++i];
    }
  }
// default values.
  if(options->op == SATISFIABLE_OPTIONS__NONE) options->op = SATISFIABLE_OPTIONS__DEFAULT;
  if(options->path_src == NULL){ 
    satisfiable_error_report(SATISFIABLE_ERROR__PARSE_MISSING_SOURCE_FILE, SATISFIABLE_ERROR__PARSE, NULL, 0, 0, error);
    satisfiable_options_destroy(options, dynamically_allocated);
    return NULL;
  }
/*
  if(
    options->op == whatever options aren't implemented yet.
  ){
    satisfiable_error_report(SATISFIABLE_ERROR__UNIMPLEMENTED_OPTION, 0, 0, error);
    free(options);
    return NULL;
  }
*/
  return options;
}

#endif
