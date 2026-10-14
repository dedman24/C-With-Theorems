#ifndef SATISFIABLE_COMMON_WARN_H_INCLUDED
#define SATISFIABLE_COMMON_WARN_H_INCLUDED

// this library handles warnings.

#include "../fposT.h"                   // fpos type.
#include "../linking/rc.h"              // reference-counted strings.
#include "../ansi_colours.h"            // terminal colours.
#include "../linking/vfastarr.h"        // array type & ops.
#include "../../parse/version.h"        // SATISFIABLE_VERSION_STR.

// stdlib includes.
#include "stdio.h"                      // puts, printf.
#include "string.h"                     // memory, string handling ops.
#include "stdbool.h"                    // bool.

typedef enum{
  SATISFIABLE_WARNING__NONE,
} satisfiable_warning_typeT;

typedef enum{
  SATISFIABLE_WARNING__COMPILER,
  SATISFIABLE_WARNING__TYPECHECKER,
  SATISFIABLE_WARNING__PARSE
} satisfiable_warning_categoryT;

const char* const restrict satisfiable_warning_strings[] = {
  [SATISFIABLE_WARNING__NONE]                         = "no warning, you shouldn't be seeing this. contact devs.\n",
};

typedef struct{
  char* message;
  satisfiable_warning_categoryT category;
  rcT* path;
  satisfiable_fposT line;
  satisfiable_fposT offset;
} satisfiable_warningT;

static void satisfiable_warning_init(
  satisfiable_warningT* const restrict warning,
  char* const restrict message,
  const satisfiable_warning_categoryT category,
  rcT* const restrict path,
  const satisfiable_fposT line,
  const satisfiable_fposT offset
){
  warning->message = message;
  warning->category = category;
  if(path) rc_own(path);
  warning->path = path;
  warning->line = line;
  warning->offset = offset;
}

typedef struct{
  satisfiable_warningT* warnings;
} satisfiable_warning_ctxT;

static satisfiable_warning_ctxT* satisfiable_warning_ctx_init(satisfiable_warning_ctxT* restrict ctx){
  if(!ctx) ctx = malloc(sizeof(*ctx));
  ctx->warnings = vfastarr_init(sizeof(*ctx->warnings));
  return ctx;
}

static void satisfiable_warning_ctx_destroy(satisfiable_warning_ctxT* restrict ctx, const bool freectx){
  const size_t cnt_err = vfastarr_elcnt(ctx->warnings, sizeof(*ctx->warnings));
  for(size_t i = 0; i < cnt_err; i++){
    free(ctx->warnings[i].message);
    rc_disown(ctx->warnings[i].path);
  }

  vfastarr_destroy(ctx->warnings);
  free(ctx);
}

static void satisfiable_warning_print(satisfiable_warning_ctxT* const restrict ctx, const bool freectx){
  const size_t cnt_err = vfastarr_elcnt(ctx->warnings, sizeof(*ctx->warnings));

  for(size_t i = 0; i < cnt_err; i++){
    if(ctx->warnings[i].path){
      fputs(ANSI_COLOUR_CYAN, stderr);
      fputs(ctx->warnings[i].path->str, stderr);
      if(ctx->warnings[i].category == SATISFIABLE_WARNING__COMPILER)
        fprintf(stderr, ":%u:%u" ANSI_COLOUR_RESET, ctx->warnings[i].line, ctx->warnings[i].offset);
    }
    fputs(ANSI_COLOUR_RED "WARNING: " ANSI_COLOUR_RESET, stderr);    
    fputs(ctx->warnings[i].message, stderr);
  }

  satisfiable_warning_ctx_destroy(ctx, freectx);
}

static void satisfiable_warning_report(
  const satisfiable_warning_typeT type, const satisfiable_warning_categoryT category, rcT* const restrict path,
  const satisfiable_fposT line, const satisfiable_fposT offset,
  satisfiable_warning_ctxT* const restrict ctx
){
  const size_t len_message = strlen(satisfiable_warning_strings[type]);
  char* const restrict message = malloc(len_message + 1);
  strcpy(message, satisfiable_warning_strings[type]);

  const size_t idx_final = vfastarr_elcnt(ctx->warnings, sizeof(*ctx->warnings));
  ctx->warnings = vfastarr_extend(ctx->warnings, 1, sizeof(*ctx->warnings));
  satisfiable_warning_init(ctx->warnings + idx_final, message, category, path, line, offset);
}

static void satisfiable_warning_report__u32(
  const satisfiable_warning_typeT type, const satisfiable_warning_categoryT category, rcT* const restrict path,
  const satisfiable_fposT line, const satisfiable_fposT offset,
  satisfiable_warning_ctxT* const restrict ctx,
  const uint32_t value
){
  const size_t len_message = strlen(satisfiable_warning_strings[type]) + 2*sizeof(value);
  char* const restrict message = malloc(len_message + 1);
  sprintf(message, satisfiable_warning_strings[type], value);

  const size_t idx_final = vfastarr_elcnt(ctx->warnings, sizeof(*ctx->warnings));
  ctx->warnings = vfastarr_extend(ctx->warnings, 1, sizeof(*ctx->warnings));
  satisfiable_warning_init(ctx->warnings + idx_final, message, category, path, line, offset);
}

static void satisfiable_warning_report__str(
  const satisfiable_warning_typeT type, const satisfiable_warning_categoryT category, rcT* const restrict path,
  const satisfiable_fposT line, const satisfiable_fposT offset,
  satisfiable_warning_ctxT* const restrict ctx,
  const char* const restrict string
){
  const size_t len_message = strlen(satisfiable_warning_strings[type]) + strlen(string);
  char* const restrict message = malloc(len_message + 1);
  sprintf(message, satisfiable_warning_strings[type], string);

  const size_t idx_final = vfastarr_elcnt(ctx->warnings, sizeof(*ctx->warnings));
  ctx->warnings = vfastarr_extend(ctx->warnings, 1, sizeof(*ctx->warnings));
  satisfiable_warning_init(ctx->warnings + idx_final, message, category, path, line, offset);
}

#endif
