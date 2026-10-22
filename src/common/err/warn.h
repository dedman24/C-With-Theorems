#ifndef LTH_COMMON_WARN_H_INCLUDED
#define LTH_COMMON_WARN_H_INCLUDED

// this library handles warnings.

#include "../fposT.h"                   // fpos type.
#include "../linking/rc.h"              // reference-counted strings.
#include "../ansi_colours.h"            // terminal colours.
#include "../linking/vfastarr.h"        // array type & ops.
#include "../../parse/version.h"        // LTH_VERSION_STR.

// stdlib includes.
#include "stdio.h"                      // puts, printf.
#include "string.h"                     // memory, string handling ops.
#include "stdbool.h"                    // bool.

typedef enum{
  LTH_WARNING__NONE,
} lth_warning_typeT;

typedef enum{
  LTH_WARNING__COMPILER,
  LTH_WARNING__TYPECHECKER,
  LTH_WARNING__PARSE
} lth_warning_categoryT;

const char* const restrict lth_warning_strings[] = {
  [LTH_WARNING__NONE]                         = "no warning, you shouldn't be seeing this. contact devs.\n",
};

typedef struct{
  char* message;
  lth_warning_categoryT category;
  rcT* path;
  lth_fposT line;
  lth_fposT offset;
} lth_warningT;

static void lth_warning_init(
  lth_warningT* const restrict warning,
  char* const restrict message,
  const lth_warning_categoryT category,
  rcT* const restrict path,
  const lth_fposT line,
  const lth_fposT offset
){
  warning->message = message;
  warning->category = category;
  if(path) rc_own(path);
  warning->path = path;
  warning->line = line;
  warning->offset = offset;
}

typedef struct{
  lth_warningT* warnings;
} lth_warning_ctxT;

static lth_warning_ctxT* lth_warning_ctx_init(lth_warning_ctxT* restrict ctx){
  if(!ctx) ctx = malloc(sizeof(*ctx));
  ctx->warnings = vfastarr_init(sizeof(*ctx->warnings));
  return ctx;
}

static void lth_warning_ctx_destroy(lth_warning_ctxT* restrict ctx, const bool freectx){
  const size_t cnt_err = vfastarr_elcnt(ctx->warnings, sizeof(*ctx->warnings));
  for(size_t i = 0; i < cnt_err; i++){
    free(ctx->warnings[i].message);
    rc_disown(ctx->warnings[i].path);
  }

  vfastarr_destroy(ctx->warnings);
  free(ctx);
}

static void lth_warning_print(lth_warning_ctxT* const restrict ctx, const bool freectx){
  const size_t cnt_err = vfastarr_elcnt(ctx->warnings, sizeof(*ctx->warnings));

  for(size_t i = 0; i < cnt_err; i++){
    if(ctx->warnings[i].path){
      fputs(ANSI_COLOUR_CYAN, stderr);
      fputs(ctx->warnings[i].path->str, stderr);
      if(ctx->warnings[i].category == LTH_WARNING__COMPILER)
        fprintf(stderr, ":%u:%u" ANSI_COLOUR_RESET, ctx->warnings[i].line, ctx->warnings[i].offset);
    }
    fputs(ANSI_COLOUR_RED "WARNING: " ANSI_COLOUR_RESET, stderr);    
    fputs(ctx->warnings[i].message, stderr);
  }

  lth_warning_ctx_destroy(ctx, freectx);
}

static void lth_warning_report(
  const lth_warning_typeT type, const lth_warning_categoryT category, rcT* const restrict path,
  const lth_fposT line, const lth_fposT offset,
  lth_warning_ctxT* const restrict ctx
){
  const size_t len_message = strlen(lth_warning_strings[type]);
  char* const restrict message = malloc(len_message + 1);
  strcpy(message, lth_warning_strings[type]);

  const size_t idx_final = vfastarr_elcnt(ctx->warnings, sizeof(*ctx->warnings));
  ctx->warnings = vfastarr_extend(ctx->warnings, 1, sizeof(*ctx->warnings));
  lth_warning_init(ctx->warnings + idx_final, message, category, path, line, offset);
}

static void lth_warning_report__u32(
  const lth_warning_typeT type, const lth_warning_categoryT category, rcT* const restrict path,
  const lth_fposT line, const lth_fposT offset,
  lth_warning_ctxT* const restrict ctx,
  const uint32_t value
){
  const size_t len_message = strlen(lth_warning_strings[type]) + 2*sizeof(value);
  char* const restrict message = malloc(len_message + 1);
  sprintf(message, lth_warning_strings[type], value);

  const size_t idx_final = vfastarr_elcnt(ctx->warnings, sizeof(*ctx->warnings));
  ctx->warnings = vfastarr_extend(ctx->warnings, 1, sizeof(*ctx->warnings));
  lth_warning_init(ctx->warnings + idx_final, message, category, path, line, offset);
}

static void lth_warning_report__str(
  const lth_warning_typeT type, const lth_warning_categoryT category, rcT* const restrict path,
  const lth_fposT line, const lth_fposT offset,
  lth_warning_ctxT* const restrict ctx,
  const char* const restrict string
){
  const size_t len_message = strlen(lth_warning_strings[type]) + strlen(string);
  char* const restrict message = malloc(len_message + 1);
  sprintf(message, lth_warning_strings[type], string);

  const size_t idx_final = vfastarr_elcnt(ctx->warnings, sizeof(*ctx->warnings));
  ctx->warnings = vfastarr_extend(ctx->warnings, 1, sizeof(*ctx->warnings));
  lth_warning_init(ctx->warnings + idx_final, message, category, path, line, offset);
}

#endif
