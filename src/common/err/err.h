#ifndef CTH_COMMON_ERR_H_INCLUDED
#define CTH_COMMON_ERR_H_INCLUDED

// this library handles errors.

#include "../fposT.h"                   // fpos type.
#include "../variables.h"               // types.
#include "../linking/rc.h"              // reference-counted strings.
#include "../ansi_colours.h"            // terminal colours.
#include "../linking/vfastarr.h"        // array type & ops.
#include "../../parse/version.h"        // CTH_VERSION_STR.

// stdlib includes.
#include "stdio.h"                      // puts, printf.
#include "string.h"                     // memory, string handling ops.
#include "stdbool.h"                    // bool.

// enum holding all impossibilities.
// on adding impossibilities:
//  only add them at the end.
//  when removing impossibilities, do not alter values of old impossibilities; error codes should stay consistent across versions.
enum{
  CTH_IMPOSSIBILITY__PARSE_OPTION             = 0,
  CTH_IMPOSSIBILITY__NAME_H_DEFAULT           = 1,
  CTH_IMPOSSIBILITY__OP_TYPE                  = 2,
  CTH_IMPOSSIBILITY__VARIABLE_TYPE            = 3,
  CTH_IMPOSSIBILITY__TOKENISER_STATE_NAME     = 4,
  CTH_IMPOSSIBILITY__ASSIGNMENT_S0            = 5,
  CTH_IMPOSSIBILITY__MISSING_SRC              = 6,
};

// lovely way of handling errors, isn't it?
typedef enum{
  CTH_ERROR__NONE,
  CTH_ERROR__IMPOSSIBILITY,

  CTH_ERROR__AST_TOO_FEW_OPERANDS,

  CTH_ERROR__ASSIGNMENT_RVALUE,

  CTH_ERROR__TOKENISER_UNKNOWN_TOKEN,
  CTH_ERROR__TOKENISER_ADJACENT_OPERANDS,

  CTH_ERROR__INDENTATION_DEPTH_NEGATIVE,
  CTH_ERROR__INDENTATION_DEPTH_TOO_HIGH,

  CTH_ERROR__VALUE_UNDEFINED,
  CTH_ERROR__VALUE_UNASSIGNED,
  CTH_ERROR__VALUE_TOO_FEW_BYTES,
  CTH_ERROR__VALUE_CHAR_TOO_LONG,
  CTH_ERROR__VALUE_CHAR_TOO_SHORT,
  CTH_ERROR__VALUE_CHAR_UNFINISHED,
  CTH_ERROR__VALUE_STRING_UNFINISHED,
  CTH_ERROR__VALUE_FORMAT_UNKNOWN,
  CTH_ERROR__VALUE_FORMAT_UNIMPLEMENTED,
  CTH_ERROR__VALUE_UNKNOWN_ESCAPE_SEQUENCE,

  CTH_ERROR__TYPEIS_SECOND_ARG,
  CTH_ERROR__TYPE_INCOMPATIBLE_OP,
  CTH_ERROR__TYPE_INCOMPATIBLE_RETURN,

  CTH_ERROR__INTERPRETER_AST_TYPE,
  CTH_ERROR__INTERPRETER_DIVIDE_BY_ZERO,
  CTH_ERROR__INTERPRETER_OP_UNIMPLEMENTED,
  CTH_ERROR__INTERPRETER_TYPE_UNSUPPORTED,
  CTH_ERROR__INTERPRETER_VARIABLE_MISSING,

  CTH_ERROR__INCLUDE_NO_LEADING_QUOTES,
  CTH_ERROR__INCLUDE_NO_TRAILING_QUOTES,
  CTH_ERROR__INCLUDE_SELF_REFERENCE,
  CTH_ERROR__INCLUDE_UNKNOWN_FILE,

  CTH_ERROR__PARSE_UNIMPLEMENTED_OPTION,
  CTH_ERROR__PARSE_REPEATED_OPTION,
  CTH_ERROR__PARSE_UNKNOWN_OPTION,
  CTH_ERROR__PARSE_MISSING_SOURCE_FILE,
  CTH_ERROR__PARSE_UNKNOWN_SOURCE_FILE,
} cth_error_typeT;

typedef enum{
  CTH_ERROR__COMPILER,
  CTH_ERROR__COMPILER_DESTRUCTION,            // while ctx is being destroyed.
  CTH_ERROR__TYPECHECKER,
  CTH_ERROR__PARSE
} cth_error_categoryT;

const char* const restrict cth_error_strings[] = {
  [CTH_ERROR__NONE]                           = "no error, you shouldn't be seeing this. contact devs.\n",
  [CTH_ERROR__IMPOSSIBILITY]                  = "an 'impossibility', something that shouldn't happen, just did "
                                                 "(C With Theorems version " CTH_VERSION_STR "). "
                                                 "contact devs. " 
                                                 ANSI_COLOUR_RED "ERROR CODE: %08xh" ANSI_COLOUR_RESET ".\n",

  [CTH_ERROR__AST_TOO_FEW_OPERANDS]           = "too few operands present for op to be converted into abstract syntax tree format.\n",

  [CTH_ERROR__ASSIGNMENT_RVALUE]              = "assignment to rvalue is invalid.\n",

  [CTH_ERROR__TOKENISER_UNKNOWN_TOKEN]        = "unknown token met during tokenisation; possibly an issue with the compiler.\n",
  [CTH_ERROR__TOKENISER_ADJACENT_OPERANDS]    = "two operands are adjacent with no operator between them, and the former isn't a type.\n",

  [CTH_ERROR__INDENTATION_DEPTH_NEGATIVE]     = "scope depth is negative, due to extra '}'.\n",
  [CTH_ERROR__INDENTATION_DEPTH_TOO_HIGH]     = "at end of tokenisation, indentation depth is not 0.\n",

  [CTH_ERROR__TYPEIS_SECOND_ARG]              = "in '::', second argument is not type.\n",
  [CTH_ERROR__TYPE_INCOMPATIBLE_OP]           = "in expression, type %s is incompatible with type %s.\n",
  [CTH_ERROR__TYPE_INCOMPATIBLE_RETURN]       = "expected return type %s is incompatible with type %s.\n",

  [CTH_ERROR__INTERPRETER_AST_TYPE]           = "ast node is of type %d, while interpreter dispatched op of type %d. contact devs.\n",
  [CTH_ERROR__INTERPRETER_DIVIDE_BY_ZERO]     = "divide-by-zero happened.\n",
  [CTH_ERROR__INTERPRETER_OP_UNIMPLEMENTED]   = "operator is unimplemented.\n",
  [CTH_ERROR__INTERPRETER_TYPE_UNSUPPORTED]   = "type '%s' does not support specified operation.\n",
  [CTH_ERROR__INTERPRETER_VARIABLE_MISSING]   = "could not find variable in variable_ctxT. contact devs.\n",

  [CTH_ERROR__VALUE_UNDEFINED]                = "value is used before being defined.\n",        // definition ~ giving type to variable through '::'.
  [CTH_ERROR__VALUE_UNASSIGNED]               = "value is used before being assigned.\n",       // assignment ~ giving value to variable through '='.
  [CTH_ERROR__VALUE_TOO_FEW_BYTES]            = "immediate is read, but not entirely; only %u bytes are.\n",
  [CTH_ERROR__VALUE_CHAR_TOO_LONG]            = "single quotes hold too many characters.\n",
  [CTH_ERROR__VALUE_CHAR_TOO_SHORT]           = "data specified by single quotes does not occupy all of quotes.\n",
  [CTH_ERROR__VALUE_CHAR_UNFINISHED]          = "char is not properly terminated by final single quote character.\n",
  [CTH_ERROR__VALUE_STRING_UNFINISHED]        = "string is not properly terminated by final double quote character.\n",
  [CTH_ERROR__VALUE_FORMAT_UNKNOWN]           = "value assumes invalid format.\n",
  [CTH_ERROR__VALUE_FORMAT_UNIMPLEMENTED]     = "value assumes format that not yet implemented in the compiler.\n",   // lmfao what a copout.
  [CTH_ERROR__VALUE_UNKNOWN_ESCAPE_SEQUENCE]  = "unknown escape sequence.\n",

  [CTH_ERROR__INCLUDE_NO_LEADING_QUOTES]      = "'include' not followed by double-quote symbol (\").\n",
  [CTH_ERROR__INCLUDE_NO_TRAILING_QUOTES]     = "path specified by 'include' not followed by double-quote symbol (\").\n",
  [CTH_ERROR__INCLUDE_SELF_REFERENCE]         = "source file includes itself.\n",
  [CTH_ERROR__INCLUDE_UNKNOWN_FILE]           = "in 'include', file '%s' does not exist.\n",

  [CTH_ERROR__PARSE_UNIMPLEMENTED_OPTION]     = "command-line option '%s' is not implemented yet.\n",
  [CTH_ERROR__PARSE_REPEATED_OPTION]          = "command-line option '%s' is repeated twice.\n",
  [CTH_ERROR__PARSE_UNKNOWN_OPTION]           = "command-line option '%s' was not recognised by compiler.\n",
  [CTH_ERROR__PARSE_MISSING_SOURCE_FILE]      = "source file is missing.\n",
  [CTH_ERROR__PARSE_UNKNOWN_SOURCE_FILE]      = "source file '%s' could not be found.\n"
};

typedef struct{
  char* message;
  cth_error_categoryT category;
  rcT* path;
  cth_fposT line;
  cth_fposT offset;
} cth_errorT;

static void cth_error_init(
  cth_errorT* const restrict error,
  char* const restrict message,
  const cth_error_categoryT category,
  rcT* const restrict path,
  const cth_fposT line,
  const cth_fposT offset
){
  error->message = message;
  error->category = category;
  if(path) rc_own(path);
  error->path = path;
  error->line = line;
  error->offset = offset;
}

typedef struct{
  cth_errorT* errors;
} cth_error_ctxT;

static cth_error_ctxT* cth_error_ctx_init(cth_error_ctxT* restrict ctx){
  if(!ctx) ctx = malloc(sizeof(*ctx));
  ctx->errors = vfastarr_init(sizeof(*ctx->errors));
  return ctx;
}

static void cth_error_ctx_destroy(cth_error_ctxT* restrict ctx, const bool freectx){
  const size_t cnt_err = vfastarr_elcnt(ctx->errors, sizeof(*ctx->errors));
  for(size_t i = 0; i < cnt_err; i++){
    free(ctx->errors[i].message);
    if(ctx->errors[i].path)
      rc_disown(ctx->errors[i].path);
  }

  vfastarr_destroy(ctx->errors);
  if(freectx) free(ctx);
}

static void cth_error_print(cth_error_ctxT* const restrict ctx, const bool freectx){
  const size_t cnt_err = vfastarr_elcnt(ctx->errors, sizeof(*ctx->errors));

  for(size_t i = 0; i < cnt_err; i++){
    fputs(ANSI_COLOUR_RED "ERROR" ANSI_COLOUR_RESET, stderr);
    if(ctx->errors[i].path){
      fputs(" (", stderr);
      fputs(ANSI_COLOUR_CYAN, stderr);
      fputs(ctx->errors[i].path->str, stderr);
      if(ctx->errors[i].category == CTH_ERROR__COMPILER)
        fprintf(stderr, ":%u:%u" ANSI_COLOUR_RESET, ctx->errors[i].line, ctx->errors[i].offset);
      fputc(')', stderr);
    }
    fputs(": ", stderr);
    fputs(ctx->errors[i].message, stderr);
  }

  cth_error_ctx_destroy(ctx, freectx);
}

static void cth_error_report(
  const cth_error_typeT type, const cth_error_categoryT category, rcT* const restrict path,
  const cth_fposT line, const cth_fposT offset,
  cth_error_ctxT* const restrict ctx
){
  const size_t len_message = strlen(cth_error_strings[type]);
  char* const restrict message = malloc(len_message + 1);
  strcpy(message, cth_error_strings[type]);

  const size_t idx_final = vfastarr_elcnt(ctx->errors, sizeof(*ctx->errors));
  ctx->errors = vfastarr_extend(ctx->errors, 1, sizeof(*ctx->errors));
  cth_error_init(ctx->errors + idx_final, message, category, path, line, offset);
}

static void cth_error_report__u32(
  const cth_error_typeT type, const cth_error_categoryT category, rcT* const restrict path,
  const cth_fposT line, const cth_fposT offset,
  cth_error_ctxT* const restrict ctx,
  const uint32_t value
){
  const size_t len_message = strlen(cth_error_strings[type]) + 2*sizeof(value);
  char* const restrict message = malloc(len_message + 1);
  sprintf(message, cth_error_strings[type], value);

  const size_t idx_final = vfastarr_elcnt(ctx->errors, sizeof(*ctx->errors));
  ctx->errors = vfastarr_extend(ctx->errors, 1, sizeof(*ctx->errors));
  cth_error_init(ctx->errors + idx_final, message, category, path, line, offset);
}

static void cth_error_report__u32_2(
  const cth_error_typeT type, const cth_error_categoryT category, rcT* const restrict path,
  const cth_fposT line, const cth_fposT offset,
  cth_error_ctxT* const restrict ctx,
  const uint32_t value0, const uint32_t value1
){
  const size_t len_message = strlen(cth_error_strings[type]) + 2*sizeof(value0) + 2*sizeof(value1);
  char* const restrict message = malloc(len_message + 1);
  sprintf(message, cth_error_strings[type], value0, value1);

  const size_t idx_final = vfastarr_elcnt(ctx->errors, sizeof(*ctx->errors));
  ctx->errors = vfastarr_extend(ctx->errors, 1, sizeof(*ctx->errors));
  cth_error_init(ctx->errors + idx_final, message, category, path, line, offset);
}

static void cth_error_report__str(
  const cth_error_typeT type, const cth_error_categoryT category, rcT* const restrict path,
  const cth_fposT line, const cth_fposT offset,
  cth_error_ctxT* const restrict ctx,
  const char* const restrict string
){
  const size_t len_message = strlen(cth_error_strings[type]) + strlen(string);
  char* const restrict message = malloc(len_message + 1);
  sprintf(message, cth_error_strings[type], string);

  const size_t idx_final = vfastarr_elcnt(ctx->errors, sizeof(*ctx->errors));
  ctx->errors = vfastarr_extend(ctx->errors, 1, sizeof(*ctx->errors));
  cth_error_init(ctx->errors + idx_final, message, category, path, line, offset);
}

static void cth_error_report__str_2(
  const cth_error_typeT type, const cth_error_categoryT category, rcT* const restrict path,
  const cth_fposT line, const cth_fposT offset,
  cth_error_ctxT* const restrict ctx,
  const char* const string0, const char* const string1
){
  const size_t len_message = strlen(cth_error_strings[type]) + strlen(string0) + strlen(string1);
  char* const restrict message = malloc(len_message + 1);
  sprintf(message, cth_error_strings[type], string0, string1);

  const size_t idx_final = vfastarr_elcnt(ctx->errors, sizeof(*ctx->errors));
  ctx->errors = vfastarr_extend(ctx->errors, 1, sizeof(*ctx->errors));
  cth_error_init(ctx->errors + idx_final, message, category, path, line, offset);
}

static void cth_error_report__type(
  const cth_error_typeT type, const cth_error_categoryT category, rcT* const restrict path,
  const cth_fposT line, const cth_fposT offset,
  cth_error_ctxT* const restrict ctx,
  const cth_nameT type_data
){
  const size_t len_message = strlen(cth_error_strings[type]) + strlen(cth_type__strings[type_data]);
  char* const restrict message = malloc(len_message + 1);
  sprintf(message, cth_error_strings[type], cth_type__strings[type_data]);

  const size_t idx_final = vfastarr_elcnt(ctx->errors, sizeof(*ctx->errors));
  ctx->errors = vfastarr_extend(ctx->errors, 1, sizeof(*ctx->errors));
  cth_error_init(ctx->errors + idx_final, message, category, path, line, offset);
}

static void cth_error_report__type_2(
  const cth_error_typeT type, const cth_error_categoryT category, rcT* const restrict path,
  const cth_fposT line, const cth_fposT offset,
  cth_error_ctxT* const restrict ctx,
  const cth_nameT t0, const cth_nameT t1
){
  const size_t len_message = strlen(cth_error_strings[type]) + strlen(cth_type__strings[t0]) + strlen(cth_type__strings[t1]);
  char* const restrict message = malloc(len_message + 1);
  sprintf(message, cth_error_strings[type], cth_type__strings[t0], cth_type__strings[t1]);

  const size_t idx_final = vfastarr_elcnt(ctx->errors, sizeof(*ctx->errors));
  ctx->errors = vfastarr_extend(ctx->errors, 1, sizeof(*ctx->errors));
  cth_error_init(ctx->errors + idx_final, message, category, path, line, offset);
}

#endif
