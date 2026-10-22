#ifndef LTH_COMMON_ERR_H_INCLUDED
#define LTH_COMMON_ERR_H_INCLUDED

// this library handles errors.

#include "../fposT.h"                   // fpos type.
#include "../variables.h"               // types.
#include "../linking/rc.h"              // reference-counted strings.
#include "../ansi_colours.h"            // terminal colours.
#include "../linking/vfastarr.h"        // array type & ops.
#include "../../parse/version.h"        // LTH_VERSION_STR.

// stdlib includes.
#include "stdio.h"                      // puts, printf.
#include "string.h"                     // memory, string handling ops.
#include "stdbool.h"                    // bool.

// enum holding all impossibilities.
enum{
  LTH_IMPOSSIBILITY__PARSE_OPTION,
  LTH_IMPOSSIBILITY__NAME_H_DEFAULT,
  LTH_IMPOSSIBILITY__OP_TYPE
};

// lovely way of handling errors, isn't it?
typedef enum{
  LTH_ERROR__NONE,
  LTH_ERROR__IMPOSSIBILITY,

  LTH_ERROR__INDENTATION_DEPTH_NEGATIVE,
  LTH_ERROR__INDENTATION_DEPTH_TOO_HIGH,

  LTH_ERROR__VALUE_UNDEFINED,
  LTH_ERROR__VALUE_UNASSIGNED,
  LTH_ERROR__VALUE_TOO_FEW_BYTES,
  LTH_ERROR__VALUE_CHAR_TOO_LONG,
  LTH_ERROR__VALUE_CHAR_TOO_SHORT,
  LTH_ERROR__VALUE_CHAR_UNFINISHED,
  LTH_ERROR__VALUE_STRING_UNFINISHED,
  LTH_ERROR__VALUE_FORMAT_UNKNOWN,
  LTH_ERROR__VALUE_FORMAT_UNIMPLEMENTED,
  LTH_ERROR__VALUE_UNKNOWN_ESCAPE_SEQUENCE,

  LTH_ERROR__TYPE_INCOMPATIBLE_OP,
  LTH_ERROR__TYPE_INCOMPATIBLE_RETURN,

  LTH_ERROR__INTERPRETER_DIVIDE_BY_ZERO,
  LTH_ERROR__INTERPRETER_OP_UNIMPLEMENTED,
  LTH_ERROR__INTERPRETER_TOO_FEW_OPERANDS,
  LTH_ERROR__INTERPRETER_TYPE_UNSUPPORTED,
  LTH_ERROR__INTERPRETER_VARIABLE_MISSING,
  LTH_ERROR__INTERPRETER_ADJACENT_OPERANDS,

  LTH_ERROR__INCLUDE_NO_LEADING_QUOTES,
  LTH_ERROR__INCLUDE_NO_TRAILING_QUOTES,
  LTH_ERROR__INCLUDE_SELF_REFERENCE,
  LTH_ERROR__INCLUDE_UNKNOWN_FILE,

  LTH_ERROR__PARSE_UNIMPLEMENTED_OPTION,
  LTH_ERROR__PARSE_REPEATED_OPTION,
  LTH_ERROR__PARSE_UNKNOWN_OPTION,
  LTH_ERROR__PARSE_MISSING_SOURCE_FILE,
  LTH_ERROR__PARSE_UNKNOWN_SOURCE_FILE,
} lth_error_typeT;

typedef enum{
  LTH_ERROR__COMPILER,
  LTH_ERROR__COMPILER_DESTRUCTION,            // while ctx is being destroyed.
  LTH_ERROR__TYPECHECKER,
  LTH_ERROR__PARSE
} lth_error_categoryT;

const char* const restrict lth_error_strings[] = {
  [LTH_ERROR__NONE]                           = "no error, you shouldn't be seeing this. contact devs.\n",
  [LTH_ERROR__IMPOSSIBILITY]                  = "an 'impossibility', something that shouldn't happen, just did "
                                                        "(Satisfiable version " LTH_VERSION_STR "). "
                                                        "contact devs. " 
                                                        ANSI_COLOUR_RED "ERROR CODE: %08xh" ANSI_COLOUR_RESET ".\n",

  [LTH_ERROR__INDENTATION_DEPTH_NEGATIVE]     = "scope depth is negative, due to extra '}'.\n",
  [LTH_ERROR__INDENTATION_DEPTH_TOO_HIGH]     = "at end of tokenisation, indentation depth is not 0.\n",

  [LTH_ERROR__TYPE_INCOMPATIBLE_OP]           = "in expression, type %s is incompatible with type %s.\n",
  [LTH_ERROR__TYPE_INCOMPATIBLE_RETURN]       = "expected return type %s is incompatible with type %s.\n",

  [LTH_ERROR__INTERPRETER_DIVIDE_BY_ZERO]     = "divide-by-zero happened.\n",
  [LTH_ERROR__INTERPRETER_OP_UNIMPLEMENTED]   = "operator is unimplemented.\n",
  [LTH_ERROR__INTERPRETER_TOO_FEW_OPERANDS]   = "too few operands present for op to be executed.\n",
  [LTH_ERROR__INTERPRETER_TYPE_UNSUPPORTED]   = "type '%s' does not support specified operation.\n",
  [LTH_ERROR__INTERPRETER_VARIABLE_MISSING]   = "could not find variable in variable_ctxT. contact devs.\n",
  [LTH_ERROR__INTERPRETER_ADJACENT_OPERANDS]  = "two operands are adjacent with no operator between them.\n",

  [LTH_ERROR__VALUE_UNDEFINED]                = "value is used before being defined.\n",        // definition ~ giving type to variable through ':'.
  [LTH_ERROR__VALUE_UNASSIGNED]               = "value is used before being assigned.\n",       // assignment ~ giving value to variable through '='.
  [LTH_ERROR__VALUE_TOO_FEW_BYTES]            = "immediate is read, but not entirely; only %u bytes are.\n",
  [LTH_ERROR__VALUE_CHAR_TOO_LONG]            = "single quotes hold too many characters.\n",
  [LTH_ERROR__VALUE_CHAR_TOO_SHORT]           = "data specified by single quotes does not occupy all of quotes.\n",
  [LTH_ERROR__VALUE_CHAR_UNFINISHED]          = "char is not properly terminated by final single quote character.\n",
  [LTH_ERROR__VALUE_STRING_UNFINISHED]        = "string is not properly terminated by final double quote character.\n",
  [LTH_ERROR__VALUE_FORMAT_UNKNOWN]           = "value assumes invalid format.\n",
  [LTH_ERROR__VALUE_FORMAT_UNIMPLEMENTED]     = "value assumes format that not yet implemented in the compiler.\n",   // lmfao what a copout.
  [LTH_ERROR__VALUE_UNKNOWN_ESCAPE_SEQUENCE]  = "unknown escape sequence.\n",

  [LTH_ERROR__INCLUDE_NO_LEADING_QUOTES]      = "'include' not followed by double-quote symbol (\").\n",
  [LTH_ERROR__INCLUDE_NO_TRAILING_QUOTES]     = "path specified by 'include' not followed by double-quote symbol (\").\n",
  [LTH_ERROR__INCLUDE_SELF_REFERENCE]         = "source file includes itself.\n",
  [LTH_ERROR__INCLUDE_UNKNOWN_FILE]           = "in 'include', file '%s' does not exist.\n",

  [LTH_ERROR__PARSE_UNIMPLEMENTED_OPTION]     = "command-line option '%s' is not implemented yet.\n",
  [LTH_ERROR__PARSE_REPEATED_OPTION]          = "command-line option '%s' is repeated twice.\n",
  [LTH_ERROR__PARSE_UNKNOWN_OPTION]           = "command-line option '%s' was not recognised by compiler.\n",
  [LTH_ERROR__PARSE_MISSING_SOURCE_FILE]      = "source file is missing.\n",
  [LTH_ERROR__PARSE_UNKNOWN_SOURCE_FILE]      = "source file '%s' could not be found.\n"
};

typedef struct{
  char* message;
  lth_error_categoryT category;
  rcT* path;
  lth_fposT line;
  lth_fposT offset;
} lth_errorT;

static void lth_error_init(
  lth_errorT* const restrict error,
  char* const restrict message,
  const lth_error_categoryT category,
  rcT* const restrict path,
  const lth_fposT line,
  const lth_fposT offset
){
  error->message = message;
  error->category = category;
  if(path) rc_own(path);
  error->path = path;
  error->line = line;
  error->offset = offset;
}

typedef struct{
  lth_errorT* errors;
} lth_error_ctxT;

static lth_error_ctxT* lth_error_ctx_init(lth_error_ctxT* restrict ctx){
  if(!ctx) ctx = malloc(sizeof(*ctx));
  ctx->errors = vfastarr_init(sizeof(*ctx->errors));
  return ctx;
}

static void lth_error_ctx_destroy(lth_error_ctxT* restrict ctx, const bool freectx){
  const size_t cnt_err = vfastarr_elcnt(ctx->errors, sizeof(*ctx->errors));
  for(size_t i = 0; i < cnt_err; i++){
    free(ctx->errors[i].message);
    if(ctx->errors[i].path)
      rc_disown(ctx->errors[i].path);
  }

  vfastarr_destroy(ctx->errors);
  if(freectx) free(ctx);
}

static void lth_error_print(lth_error_ctxT* const restrict ctx, const bool freectx){
  const size_t cnt_err = vfastarr_elcnt(ctx->errors, sizeof(*ctx->errors));

  for(size_t i = 0; i < cnt_err; i++){
    fputs(ANSI_COLOUR_RED "ERROR" ANSI_COLOUR_RESET, stderr);
    if(ctx->errors[i].path){
      fputs(" (", stderr);
      fputs(ANSI_COLOUR_CYAN, stderr);
      fputs(ctx->errors[i].path->str, stderr);
      if(ctx->errors[i].category == LTH_ERROR__COMPILER)
        fprintf(stderr, ":%u:%u" ANSI_COLOUR_RESET, ctx->errors[i].line, ctx->errors[i].offset);
      fputc(')', stderr);
    }
    fputs(": ", stderr);
    fputs(ctx->errors[i].message, stderr);
  }

  lth_error_ctx_destroy(ctx, freectx);
}

static void lth_error_report(
  const lth_error_typeT type, const lth_error_categoryT category, rcT* const restrict path,
  const lth_fposT line, const lth_fposT offset,
  lth_error_ctxT* const restrict ctx
){
  const size_t len_message = strlen(lth_error_strings[type]);
  char* const restrict message = malloc(len_message + 1);
  strcpy(message, lth_error_strings[type]);

  const size_t idx_final = vfastarr_elcnt(ctx->errors, sizeof(*ctx->errors));
  ctx->errors = vfastarr_extend(ctx->errors, 1, sizeof(*ctx->errors));
  lth_error_init(ctx->errors + idx_final, message, category, path, line, offset);
}

static void lth_error_report__u32(
  const lth_error_typeT type, const lth_error_categoryT category, rcT* const restrict path,
  const lth_fposT line, const lth_fposT offset,
  lth_error_ctxT* const restrict ctx,
  const uint32_t value
){
  const size_t len_message = strlen(lth_error_strings[type]) + 2*sizeof(value);
  char* const restrict message = malloc(len_message + 1);
  sprintf(message, lth_error_strings[type], value);

  const size_t idx_final = vfastarr_elcnt(ctx->errors, sizeof(*ctx->errors));
  ctx->errors = vfastarr_extend(ctx->errors, 1, sizeof(*ctx->errors));
  lth_error_init(ctx->errors + idx_final, message, category, path, line, offset);
}

static void lth_error_report__str(
  const lth_error_typeT type, const lth_error_categoryT category, rcT* const restrict path,
  const lth_fposT line, const lth_fposT offset,
  lth_error_ctxT* const restrict ctx,
  const char* const restrict string
){
  const size_t len_message = strlen(lth_error_strings[type]) + strlen(string);
  char* const restrict message = malloc(len_message + 1);
  sprintf(message, lth_error_strings[type], string);

  const size_t idx_final = vfastarr_elcnt(ctx->errors, sizeof(*ctx->errors));
  ctx->errors = vfastarr_extend(ctx->errors, 1, sizeof(*ctx->errors));
  lth_error_init(ctx->errors + idx_final, message, category, path, line, offset);
}

static void lth_error_report__type1(
  const lth_error_typeT type, const lth_error_categoryT category, rcT* const restrict path,
  const lth_fposT line, const lth_fposT offset,
  lth_error_ctxT* const restrict ctx,
  const lth_nameT type_data
){
  const size_t len_message = strlen(lth_error_strings[type]) + strlen(lth_type__strings[type_data]);
  char* const restrict message = malloc(len_message + 1);
  sprintf(message, lth_error_strings[type], lth_type__strings[type_data]);

  const size_t idx_final = vfastarr_elcnt(ctx->errors, sizeof(*ctx->errors));
  ctx->errors = vfastarr_extend(ctx->errors, 1, sizeof(*ctx->errors));
  lth_error_init(ctx->errors + idx_final, message, category, path, line, offset);
}

static void lth_error_report__type2(
  const lth_error_typeT type, const lth_error_categoryT category, rcT* const restrict path,
  const lth_fposT line, const lth_fposT offset,
  lth_error_ctxT* const restrict ctx,
  const lth_nameT t0, const lth_nameT t1
){
  const size_t len_message = strlen(lth_error_strings[type]) + strlen(lth_type__strings[t0]) + strlen(lth_type__strings[t1]);
  char* const restrict message = malloc(len_message + 1);
  sprintf(message, lth_error_strings[type], lth_type__strings[t0], lth_type__strings[t1]);

  const size_t idx_final = vfastarr_elcnt(ctx->errors, sizeof(*ctx->errors));
  ctx->errors = vfastarr_extend(ctx->errors, 1, sizeof(*ctx->errors));
  lth_error_init(ctx->errors + idx_final, message, category, path, line, offset);
}

#endif
