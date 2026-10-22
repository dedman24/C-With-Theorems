#ifndef LTH_TOKENISER_H_INCLUDED
#define LTH_TOKENISER_H_INCLUDED

#include "name.h"                   // name handling.
#include "../common/_common.h"      // tokens et cetera.

// stdlib includes.
#include "stddef.h"                 // for size_t.
#include "string.h"                 // for string manipulation functions.

typedef struct{
  lth_tokT* arr;
  lth_name_ctxT name;
  vfastbst_rcT files;
} lth_tokeniserT;

static void lth_tokeniser_init(lth_tokeniserT* const restrict tokeniser){
  lth_name_ctx_init(&tokeniser->name);
  tokeniser->arr = vfastarr_init(sizeof(*tokeniser->arr));
  tokeniser->files = VFASTBST_RC_INIT;
}

static lth_outputT lth_tokeniser_destroy(
  lth_tokeniserT* const restrict tokeniser,  lth_variable_ctxT** variables,
  lth_error_ctxT* const restrict err, rcT* const restrict path
){
  if(variables) *variables = tokeniser->name.variables;
  else lth_variable_ctx_destroy(tokeniser->name.variables, true);

  if(!lth_name_ctx_destroy(&tokeniser->name, false))
    lth_error_report(LTH_ERROR__INDENTATION_DEPTH_NEGATIVE, LTH_ERROR__COMPILER_DESTRUCTION, path, 0, 0, err);
  vfastbst_rc_destroy(&tokeniser->files);
  return (lth_outputT)tokeniser->arr;
}

// path   ~ file path itself.
// s_path ~ length of path excluding actual filename.
// err    ~ error ctx struct holding all reported during compilation.
static void lth_tokenise__single(
  const char* restrict code,
  rcT* const restrict path, const size_t s_path,
  lth_error_ctxT* const restrict err,
  lth_tokeniserT* const restrict ctx
);

// handles 'include' commands by parsing the path & pushing it to the queue, if it wasn't already.
static const char* lth_tokenise__include(
  const char* restrict code, rcT* const restrict path, const size_t s_path,
  lth_tokeniserT* const restrict tokeniser,
  lth_error_ctxT* const restrict err, lth_fposT* const restrict line, lth_fposT* const restrict offset
){
  code += strcspn(code, "\"");
  if(code[0] != '"'){     // Include isn't followed by single quotes, so errors out.
    lth_error_report(LTH_ERROR__INCLUDE_NO_LEADING_QUOTES, LTH_ERROR__COMPILER, path, *line, *offset, err);
    return code;
  }

  const size_t s_finalpath = strcspn(code, "\""); 
  code += s_finalpath;
  if(code[0] != '"'){
    lth_error_report(LTH_ERROR__INCLUDE_NO_LEADING_QUOTES, LTH_ERROR__COMPILER, path, *line, *offset, err);
    return code;
  }

  rcT* const restrict finalpath = rc_make(vfastpath_append(path->str, s_path, code, s_finalpath));
  size_t s_filename;
  char* filename = strrchr(finalpath->str, '/');
  if(!filename) s_filename = 0;
  else s_filename = s_finalpath - (filename - finalpath->str) - 1 /* correction term, because we want to exclude last '/'. */;

  if((s_finalpath - s_filename) == s_path && !strcmp(path->str, finalpath->str)){
    lth_error_report(LTH_ERROR__INCLUDE_SELF_REFERENCE, LTH_ERROR__COMPILER, path, *line, *offset, err);
    rc_disown(finalpath);
    return code;
  }

  vfastbst_rc_nodeT** const restrict success = vfastbst_rc_put_half(&tokeniser->files, finalpath);
// error out if an include's argument is ever NULL.
  lth_tokT* new = NULL;
  if(!*success){
    const lth__fileT finalcode = lth_load__map(finalpath->str, true);
    if(!finalcode.code){
      lth_error_report__str(LTH_ERROR__INCLUDE_UNKNOWN_FILE, LTH_ERROR__COMPILER, path, *line, *offset, err, finalpath->str);
      rc_disown(finalpath);
      return code;
    }

    void* const restrict old = tokeniser->arr;
    tokeniser->arr = vfastarr_init(sizeof(*tokeniser->arr));
    lth_tokenise__single(finalcode.code, finalpath, s_finalpath - s_filename, err, tokeniser);
    lth_load__unmap(finalcode);

    new = tokeniser->arr; tokeniser->arr = old;
    *success = vfastbst_rc_node_init(finalpath, (lth__underlying_tokT)new);
  }
  else
    new = (lth_tokT*)(*success)->obj;

// when processing stuff, the 'Include' token is followed by some ptr to data representing the file being included, whatever that is.
  const lth_tokT info = lth_tok_init((lth__underlying_tokT)new, *line, *offset);
  tokeniser->arr = vfastarr_append(tokeniser->arr, &info, sizeof(info));

  rc_disown(finalpath);
  return code;
}

static const char* lth_tokenise__comment(
  const char* restrict code,
  lth_fposT* const restrict line,
  lth_fposT* const restrict offset
){
  const char* restrict n_code = strchr(code, '\n');
  if(!n_code) n_code = code + strlen(code);                     // I'd use strchrnul but it's GNU only.
  else n_code++;                                                // this way it points after the '\n'.
  (*line)++;
  *offset += code - n_code;
  return n_code;
}

// tokenises code pointed at by 'code' variable.
static void lth_tokenise__single(
  const char* restrict code, rcT* const restrict path, const size_t s_path,
  lth_error_ctxT* const restrict err, lth_tokeniserT* const restrict tokeniser
){
  lth_fposT line = 0, offset = 0;

  while(1){
    size_t len;
    const lth_tok_typeT tok = lth_tok(code, &len);
// tokens that should be ignored.
    switch(tok){
      case LTH_TOK__NEWLINE:
        line++; offset = 0;
        code += len;
        continue;
      case LTH_TOK__WHITESPACE:
        break;
      case LTH_TOK__COMMENT:
        code = lth_tokenise__comment(code + len, &line, &offset);
        continue;
      case LTH_TOK__EOF:
        return;
  
      case LTH_TOK__OPENED_BRACKET:
        lth_name_depth_inc(&tokeniser->name);
        break;
      case LTH_TOK__CLOSED_BRACKET:
        if(!lth_name_depth_dec(&tokeniser->name))
          lth_error_report(LTH_ERROR__INDENTATION_DEPTH_NEGATIVE, LTH_ERROR__COMPILER, path, line, offset, err);
        break;

      case LTH_TOK__NAME:
        const lth_nameT name = lth_name_add(code, len, err, &tokeniser->name);
        tokeniser->arr = vfastarr_append(tokeniser->arr, &lth_tok_init(name, line, offset), sizeof(lth_tokT));
        break;
      case LTH_TOK__VALUE:
        const lth_nameT value = lth_value_add(code, len, &tokeniser->name, err, path, line, offset);
        tokeniser->arr = vfastarr_append(tokeniser->arr, &lth_tok_init(value, line, offset), sizeof(lth_tokT));
        break;
      case LTH_TOK__CHAR:
        const lth_nameT character = lth_char_add(code, len, &tokeniser->name, err, path, line, offset);
        tokeniser->arr = vfastarr_append(tokeniser->arr, &lth_tok_init(character, line, offset), sizeof(lth_tokT));
        break;
      case LTH_TOK__STRING:
        const lth_nameT string = lth_string_add(code, len, &tokeniser->name, err, path, line, offset);
        tokeniser->arr = vfastarr_append(tokeniser->arr, &lth_tok_init(string, line, offset), sizeof(lth_tokT));
        break;
    }
    code += len;
    offset += len;
  }
}

static lth_outputT lth_tokenise(
  const char* restrict code, const char* const restrict path, const size_t s_path,
  lth_variable_ctxT** const restrict variables, lth_error_ctxT* const restrict err
){
  rcT* const restrict rc_path = rc_make(path);
  lth_tokeniserT tokeniser; lth_tokeniser_init(&tokeniser);

// such descriptive names, clearly indicative of a cnile...
// C's only missing a defer{} statement (that doesn't require #include "stddefer.h" or some bs like that) & it'd be my favourite language.
// really it's missing only a handful of features to make it top1, but adding them would break backwards compat...
// one should make their own C standard that breaks backwards compat somewhat at the expense of a better experience.
//  or just use zig... but it isn't stable yet & I want my stuff to compile for the next thousand years.
// myC would have all features zig does, basically native C interop (not 100% backwards compatibility though) & more.
// I wouldn't like it if it didn't have dependent types, though I'd barely use them.
  lth_tokenise__single(code, rc_path, s_path, err, &tokeniser);
  lth_outputT tokenised = lth_tokeniser_destroy(&tokeniser, variables, err, rc_path);
  rc_disown(rc_path);
// result of tokenisation.
  return tokenised;
}

#endif
