#ifndef SATISFIABLE_COMMON_ERR_H_INCLUDED
#define SATISFIABLE_COMMON_ERR_H_INCLUDED

// this library handles errors.

#include "../fposT.h"                   // fpos type.
#include "../linking/rc.h"              // reference-counted strings.
#include "../ansi_colours.h"            // terminal colours.
#include "../linking/vfastarr.h"        // array type & ops.
#include "../../parse/version.h"        // SATISFIABLE_VERSION_STR.

// stdlib includes.
#include "stdio.h"                      // puts, printf.
#include "string.h"                     // memory, string handling ops.
#include "stdbool.h"                    // bool.

// enum holding all impossibilities.
enum{
  SATISFIABLE_IMPOSSIBILITY__PARSE_OPTION,
  SATISFIABLE_IMPOSSIBILITY__TOK_H_RETURN_VALUE,
};

typedef enum{
  SATISFIABLE_ERROR__NONE,
  SATISFIABLE_ERROR__IMPOSSIBILITY,
  SATISFIABLE_ERROR__VALUE_TOO_FEW_BYTES,
  SATISFIABLE_ERROR__VALUE_FORMAT_UNKNOWN,
  SATISFIABLE_ERROR__VALUE_FORMAT_UNIMPLEMENTED,
  SATISFIABLE_ERROR__INDENTATION_DEPTH_NEGATIVE,
  SATISFIABLE_ERROR__INCLUDE_NO_LEADING_QUOTES,
  SATISFIABLE_ERROR__INCLUDE_NO_TRAILING_QUOTES,
  SATISFIABLE_ERROR__INCLUDE_SELF_REFERENCE,
  SATISFIABLE_ERROR__INCLUDE_UNKNOWN_FILE,
// last bit used to mark error stuff.
  SATISFIABLE_ERROR__PARSE_UNIMPLEMENTED_OPTION,
  SATISFIABLE_ERROR__PARSE_REPEATED_OPTION,
  SATISFIABLE_ERROR__PARSE_UNKNOWN_OPTION,
  SATISFIABLE_ERROR__PARSE_MISSING_SOURCE_FILE,
  SATISFIABLE_ERROR__PARSE_UNKNOWN_SOURCE_FILE,
} satisfiable_error_typeT;

typedef enum{
  SATISFIABLE_ERROR__COMPILER,
  SATISFIABLE_ERROR__TYPECHECKER,
  SATISFIABLE_ERROR__PARSE
} satisfiable_error_categoryT;

const char* const restrict satisfiable_error_strings[] = {
  [SATISFIABLE_ERROR__NONE]                         = "no error, you shouldn't be seeing this. contact devs.\n",
  [SATISFIABLE_ERROR__IMPOSSIBILITY]                = "an 'impossibility', something that shouldn't happen, just did "
                                                      "(Satisfiable version " SATISFIABLE_VERSION_STR "). "
                                                      "contact devs. " 
                                                      ANSI_COLOUR_RED "ERROR CODE: %08xh" ANSI_COLOUR_RESET ".\n",
  [SATISFIABLE_ERROR__VALUE_TOO_FEW_BYTES]          = "immediate is read, but not entirely; only %u bytes are.\n",
  [SATISFIABLE_ERROR__VALUE_FORMAT_UNKNOWN]         = "value assumes invalid format.\n",
  [SATISFIABLE_ERROR__VALUE_FORMAT_UNIMPLEMENTED]   = "value assumes format that not yet implemented in the compiler.\n",             // lmfao what a copout.
  [SATISFIABLE_ERROR__INDENTATION_DEPTH_NEGATIVE]   = "scope depth is negative, due to extra '}'.\n",
  [SATISFIABLE_ERROR__INCLUDE_NO_LEADING_QUOTES]    = "'include' not followed by double-quote symbol (\").\n",
  [SATISFIABLE_ERROR__INCLUDE_NO_TRAILING_QUOTES]   = "path specified by 'include' not followed by double-quote symbol (\").\n",
  [SATISFIABLE_ERROR__INCLUDE_SELF_REFERENCE]       = "source file includes itself.\n",
  [SATISFIABLE_ERROR__INCLUDE_UNKNOWN_FILE]         = "in 'include', file '%s' does not exist.\n",
  [SATISFIABLE_ERROR__PARSE_UNIMPLEMENTED_OPTION]   = "command-line option '%s' is not implemented yet.\n",
  [SATISFIABLE_ERROR__PARSE_REPEATED_OPTION]        = "command-line option '%s' is repeated twice.\n",
  [SATISFIABLE_ERROR__PARSE_UNKNOWN_OPTION]         = "command-line option '%s' was not recognised by compiler.\n",
  [SATISFIABLE_ERROR__PARSE_MISSING_SOURCE_FILE]    = "source file is missing.\n",
  [SATISFIABLE_ERROR__PARSE_UNKNOWN_SOURCE_FILE]    = "source file '%s' could not be found.\n"
};

typedef struct{
  char* message;
  satisfiable_error_categoryT category;
  rcT* path;
  satisfiable_fposT line;
  satisfiable_fposT offset;
} satisfiable_errorT;

static void satisfiable_error_init(
  satisfiable_errorT* const restrict error,
  char* const restrict message,
  const satisfiable_error_categoryT category,
  rcT* const restrict path,
  const satisfiable_fposT line,
  const satisfiable_fposT offset
){
  error->message = message;
  error->category = category;
  if(path) rc_own(path);
  error->path = path;
  error->line = line;
  error->offset = offset;
}

typedef struct{
  satisfiable_errorT* errors;
} satisfiable_error_ctxT;

static satisfiable_error_ctxT* satisfiable_error_ctx_init(satisfiable_error_ctxT* restrict ctx){
  if(!ctx) ctx = malloc(sizeof(*ctx));
  ctx->errors = vfastarr_init(sizeof(*ctx->errors));
  return ctx;
}

static void satisfiable_error_ctx_destroy(satisfiable_error_ctxT* restrict ctx, const bool freectx){
  const size_t cnt_err = vfastarr_elcnt(ctx->errors, sizeof(*ctx->errors));
  for(size_t i = 0; i < cnt_err; i++){
    free(ctx->errors[i].message);
    if(ctx->errors[i].path)
      rc_disown(ctx->errors[i].path);
  }

  vfastarr_destroy(ctx->errors);
  free(ctx);
}

static void satisfiable_error_print(satisfiable_error_ctxT* const restrict ctx, const bool freectx){
  const size_t cnt_err = vfastarr_elcnt(ctx->errors, sizeof(*ctx->errors));

  for(size_t i = 0; i < cnt_err; i++){
    if(ctx->errors[i].path){
      fputs(ANSI_COLOUR_CYAN, stderr);
      fputs(ctx->errors[i].path->str, stderr);
      if(ctx->errors[i].category == SATISFIABLE_ERROR__COMPILER)
        fprintf(stderr, ":%u:%u" ANSI_COLOUR_RESET, ctx->errors[i].line, ctx->errors[i].offset);
    }
    fputs(ANSI_COLOUR_RED "ERROR: " ANSI_COLOUR_RESET, stderr);    
    fputs(ctx->errors[i].message, stderr);
  }

  satisfiable_error_ctx_destroy(ctx, freectx);
}

static void satisfiable_error_report(
  const satisfiable_error_typeT type, const satisfiable_error_categoryT category, rcT* const restrict path,
  const satisfiable_fposT line, const satisfiable_fposT offset,
  satisfiable_error_ctxT* const restrict ctx
){
  const size_t len_message = strlen(satisfiable_error_strings[type]);
  char* const restrict message = malloc(len_message + 1);
  strcpy(message, satisfiable_error_strings[type]);

  const size_t idx_final = vfastarr_elcnt(ctx->errors, sizeof(*ctx->errors));
  ctx->errors = vfastarr_extend(ctx->errors, 1, sizeof(*ctx->errors));
  satisfiable_error_init(ctx->errors + idx_final, message, category, path, line, offset);
}

static void satisfiable_error_report__u32(
  const satisfiable_error_typeT type, const satisfiable_error_categoryT category, rcT* const restrict path,
  const satisfiable_fposT line, const satisfiable_fposT offset,
  satisfiable_error_ctxT* const restrict ctx,
  const uint32_t value
){
  const size_t len_message = strlen(satisfiable_error_strings[type]) + 2*sizeof(value);
  char* const restrict message = malloc(len_message + 1);
  sprintf(message, satisfiable_error_strings[type], value);

  const size_t idx_final = vfastarr_elcnt(ctx->errors, sizeof(*ctx->errors));
  ctx->errors = vfastarr_extend(ctx->errors, 1, sizeof(*ctx->errors));
  satisfiable_error_init(ctx->errors + idx_final, message, category, path, line, offset);
}

static void satisfiable_error_report__str(
  const satisfiable_error_typeT type, const satisfiable_error_categoryT category, rcT* const restrict path,
  const satisfiable_fposT line, const satisfiable_fposT offset,
  satisfiable_error_ctxT* const restrict ctx,
  const char* const restrict string
){
  const size_t len_message = strlen(satisfiable_error_strings[type]) + strlen(string);
  char* const restrict message = malloc(len_message + 1);
  sprintf(message, satisfiable_error_strings[type], string);

  const size_t idx_final = vfastarr_elcnt(ctx->errors, sizeof(*ctx->errors));
  ctx->errors = vfastarr_extend(ctx->errors, 1, sizeof(*ctx->errors));
  satisfiable_error_init(ctx->errors + idx_final, message, category, path, line, offset);
}

#endif
