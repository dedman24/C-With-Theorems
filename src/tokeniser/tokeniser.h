#ifndef CTH_TOKENISER_H_INCLUDED
#define CTH_TOKENISER_H_INCLUDED

#include "ctx.h"                    // interpreter ctx.
#include "name.h"                   // name handling.
#include "ops/_ops.h"               // tokeniser ops.
#include "../common/_common.h"      // tokens et cetera.

// stdlib includes.
#include "stddef.h"                 // for size_t.
#include "string.h"                 // for string manipulation functions.

// path   ~ file path itself.
// s_path ~ length of path excluding actual filename.
// err    ~ error ctx struct holding all reported during compilation.
static void cth_tokenise__single(
  const char* restrict code, cth_tokeniserT* const restrict ctx,
  rcT* const restrict path, cth_error_ctxT* const restrict err
);

// handles 'include' commands by parsing the path & pushing it to the queue, if it wasn't already.
static const char* cth_tokenise__include(
  const char* restrict code, rcT* const restrict path, const size_t s_path,
  cth_tokeniserT* const restrict tokeniser,
  cth_error_ctxT* const restrict err, cth_fposT* const restrict line, cth_fposT* const restrict offset
){
  code += strcspn(code, "\"");
  if(code[0] != '"'){     // Include isn't followed by single quotes, so errors out.
    cth_error_report(CTH_ERROR__INCLUDE_NO_LEADING_QUOTES, CTH_ERROR__COMPILER, path, *line, *offset, err);
    return code;
  }

  const size_t s_finalpath = strcspn(code, "\""); 
  code += s_finalpath;
  if(code[0] != '"'){
    cth_error_report(CTH_ERROR__INCLUDE_NO_LEADING_QUOTES, CTH_ERROR__COMPILER, path, *line, *offset, err);
    return code;
  }

  rcT* const restrict finalpath = rc_make(vfastpath_append(path->str, s_path, code, s_finalpath));
  size_t s_filename;
  char* filename = strrchr(finalpath->str, '/');
  if(!filename) s_filename = 0;
  else s_filename = s_finalpath - (filename - finalpath->str) - 1 /* correction term, because we want to exclude last '/'. */;

  if((s_finalpath - s_filename) == s_path && !strcmp(path->str, finalpath->str)){
    cth_error_report(CTH_ERROR__INCLUDE_SELF_REFERENCE, CTH_ERROR__COMPILER, path, *line, *offset, err);
    rc_disown(finalpath);
    return code;
  }

  vfastbst_rc_nodeT** const restrict success = vfastbst_rc_put_half(&tokeniser->files, finalpath);
// error out if an include's argument is ever NULL.
  cth_astT* new = NULL;
  if(!*success){
    const cth__fileT finalcode = cth_load__map(finalpath->str, true);
    if(!finalcode.code){
      cth_error_report__str(CTH_ERROR__INCLUDE_UNKNOWN_FILE, CTH_ERROR__COMPILER, path, *line, *offset, err, finalpath->str);
      rc_disown(finalpath);
      return code;
    }

    void* const restrict old = tokeniser->program;
    tokeniser->program = NULL;
    cth_tokenise__single(finalcode.code, tokeniser, finalpath, err);
    cth_load__unmap(finalcode);

    new = tokeniser->program; tokeniser->program = old;
//    *success = vfastbst_rc_node_init(finalpath, (cth_nameT)new);
  }
//  else
//    new = (cth_ast_programT*)(*success)->obj;

// when processing stuff, the 'Include' token is followed by some ptr to data representing the file being included, whatever that is.
//  const cth_tokT info = (cth_nameT)new;
//  tokeniser->ast = ;

  rc_disown(finalpath);
  return code;
}

// tokenises code pointed at by 'code' variable.
static void cth_tokenise__single(
  const char* restrict code, cth_tokeniserT* const restrict tokeniser, 
  rcT* const restrict path, cth_error_ctxT* const restrict err
){
  while(*code){
    size_t len;
    const cth_tokT tok = cth_tok(code, &len);
    code = cth_tokeniser_dispatch(code, tok, len, tokeniser, path, err);
  }
}

static cth_outputT cth_tokenise(
  const char* restrict code, const char* const restrict path,
  cth_variable_ctxT** const restrict variables, cth_error_ctxT* const restrict err
){
  rcT* const restrict rc_path = rc_make(path);
  cth_tokeniserT tokeniser; cth_tokeniser_init(&tokeniser);

  cth_tokenise__single(code, &tokeniser, rc_path, err);
  cth_outputT tokenised = cth_tokeniser_destroy(&tokeniser, variables, err, rc_path);
  rc_disown(rc_path);
// result of tokenisation.
  return tokenised;
}

// tokeniser function to use specifically with the REPL.
// accepts explicit tokeniser ctx, doesn't destroy it.
// invalidates tokenised still.
static cth_outputT cth_tokenise__repl(
  const char* restrict code, const char* const restrict path, 
  cth_tokeniserT* const restrict tokeniser, cth_error_ctxT* const restrict err
){
  rcT* const restrict rc_path = rc_make(path);

  cth_tokenise__single(code, tokeniser, rc_path, err);
  rc_disown(rc_path);

  cth_outputT tokenised = (cth_outputT*)tokeniser->program;
  tokeniser->program = cth_ast_init(NULL, CTH_TOK__NONE, 0, 0, CTH_AST__PROGRAM);
// result of tokenisation.
  return tokenised;
}

#endif
