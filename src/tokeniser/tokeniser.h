#ifndef CTH_TOKENISER_H_INCLUDED
#define CTH_TOKENISER_H_INCLUDED

#include "name.h"                   // name handling.
#include "../common/_common.h"      // tokens et cetera.

// stdlib includes.
#include "stddef.h"                 // for size_t.
#include "string.h"                 // for string manipulation functions.

typedef struct{
  cth_tokT* arr;
  cth_name_ctxT name;
  vfastbst_rcT files;
} cth_tokeniserT;

static void cth_tokeniser_init(cth_tokeniserT* const restrict tokeniser){
  cth_name_ctx_init(&tokeniser->name);
  tokeniser->arr = vfastarr_init(sizeof(*tokeniser->arr));
  tokeniser->files = VFASTBST_RC_INIT;
}

static cth_outputT cth_tokeniser_destroy(
  cth_tokeniserT* const restrict tokeniser,  cth_variable_ctxT** variables,
  cth_error_ctxT* const restrict err, rcT* const restrict path
){
  if(variables) *variables = tokeniser->name.variables;
  else cth_variable_ctx_destroy(tokeniser->name.variables, true);

  if(!cth_name_ctx_destroy(&tokeniser->name, false))
    cth_error_report(CTH_ERROR__INDENTATION_DEPTH_NEGATIVE, CTH_ERROR__COMPILER_DESTRUCTION, path, 0, 0, err);
  vfastbst_rc_destroy(&tokeniser->files);
  return (cth_outputT)tokeniser->arr;
}

// path   ~ file path itself.
// s_path ~ length of path excluding actual filename.
// err    ~ error ctx struct holding all reported during compilation.
static void cth_tokenise__single(
  const char* restrict code,
  rcT* const restrict path, const size_t s_path,
  cth_error_ctxT* const restrict err,
  cth_tokeniserT* const restrict ctx
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
  cth_tokT* new = NULL;
  if(!*success){
    const cth__fileT finalcode = cth_load__map(finalpath->str, true);
    if(!finalcode.code){
      cth_error_report__str(CTH_ERROR__INCLUDE_UNKNOWN_FILE, CTH_ERROR__COMPILER, path, *line, *offset, err, finalpath->str);
      rc_disown(finalpath);
      return code;
    }

    void* const restrict old = tokeniser->arr;
    tokeniser->arr = vfastarr_init(sizeof(*tokeniser->arr));
    cth_tokenise__single(finalcode.code, finalpath, s_finalpath - s_filename, err, tokeniser);
    cth_load__unmap(finalcode);

    new = tokeniser->arr; tokeniser->arr = old;
    *success = vfastbst_rc_node_init(finalpath, (cth__underlying_tokT)new);
  }
  else
    new = (cth_tokT*)(*success)->obj;

// when processing stuff, the 'Include' token is followed by some ptr to data representing the file being included, whatever that is.
  const cth_tokT info = cth_tok_init((cth__underlying_tokT)new, *line, *offset);
  tokeniser->arr = vfastarr_append(tokeniser->arr, &info, sizeof(info));

  rc_disown(finalpath);
  return code;
}

static const char* cth_tokenise__comment_single(
  const char* restrict code,
  cth_fposT* const restrict line,
  cth_fposT* const restrict offset
){
  const char* restrict n_code = strchr(code, '\n');
  if(!n_code) n_code = code + strlen(code);                     // I'd use strchrnul but it's GNU only.
  else n_code++;                                                // this way it points after the '\n'.
  (*line)++;
  *offset += code - n_code;
  return n_code;
}

static const char* cth_tokenise__comment_multi(
  const char* restrict code,
  cth_fposT* const restrict line,
  cth_fposT* const restrict offset
){
// we want to support arbitrarily nested comments. how?
  unsigned int depth = 1;
  const char* restrict newln = strchr(code, '\n');
  const char* restrict closest_opened = strstr(code, "/*");
  const char* restrict closest_closed = strstr(code, "*/");
// congrats: the world's ugliest code!
  do{
    if(
      newln &&
      (newln < closest_closed || !closest_closed) &&
      (newln < closest_opened || !closest_opened)
    ){ 
      *line += 1; *offset = 0;
      code = newln + 1;
      newln = strchr(code, '\n');
    }
    else if(
      closest_opened &&
      (closest_opened < newln || !newln) &&
      (closest_opened < closest_closed || !closest_closed)
    ){
      *offset += closest_opened - code; depth++;
      code = closest_opened + 2 + 1;
      closest_opened = strstr(code, "/*");
    }
    else if(
      closest_closed &&
      (closest_closed < newln || !newln) &&
      (closest_closed < closest_opened || !closest_opened)
    ){
      *offset += closest_closed - code; depth--;
      code = closest_closed + 2 + 1;
      closest_closed = strstr(code, "*/");
    }
  } while(depth && code[0]);
  return code;
}

// tokenises code pointed at by 'code' variable.
static void cth_tokenise__single(
  const char* restrict code, rcT* const restrict path, const size_t s_path,
  cth_error_ctxT* const restrict err, cth_tokeniserT* const restrict tokeniser
){
  cth_fposT line = 0, offset = 0;

  while(1){
    size_t len;
    const cth_tokT tok = cth_tok(code, &len);
// tokens that should be ignored.
    switch(tok){
      case CTH_TOK__NEWLINE:
        line++; offset = 0;
        code += len;
        continue;
      case CTH_TOK__WHITESPACE:
        break;
      case CTH_TOK__OPENED_SINGLE_LINE_COMMENT:
        code = cth_tokenise__comment_single(code + len, &line, &offset);
        continue;
      case CTH_TOK__OPENED_MULTI_LINE_COMMENT:
        code = cth_tokenise__comment_multi(code + len, &line, &offset);
        continue;
      case CTH_TOK__EOF:
        return;
  
      case CTH_TOK__OPENED_CURLY_BRACKET:
        cth_name_depth_inc(&tokeniser->name);
        break;
      case CTH_TOK__CLOSED_CURLY_BRACKET:
        if(!cth_name_depth_dec(&tokeniser->name))
          cth_error_report(CTH_ERROR__INDENTATION_DEPTH_NEGATIVE, CTH_ERROR__COMPILER, path, line, offset, err);
        break;

      case CTH_TOK__NAME:
        const cth_nameT name = cth_name_add(code, len, err, &tokeniser->name);
        tokeniser->arr = vfastarr_append(tokeniser->arr, &cth_tok_init(name, line, offset), sizeof(cth_tokT));
        break;
      case CTH_TOK__VALUE:
        const cth_nameT value = cth_value_add(code, len, &tokeniser->name, err, path, line, offset);
        tokeniser->arr = vfastarr_append(tokeniser->arr, &cth_tok_init(value, line, offset), sizeof(cth_tokT));
        break;
      case CTH_TOK__CHAR:
        const cth_nameT character = cth_char_add(code, len, &tokeniser->name, err, path, line, offset);
        tokeniser->arr = vfastarr_append(tokeniser->arr, &cth_tok_init(character, line, offset), sizeof(cth_tokT));
        break;
      case CTH_TOK__STRING:
        const cth_nameT string = cth_string_add(code, len, &tokeniser->name, err, path, line, offset);
        tokeniser->arr = vfastarr_append(tokeniser->arr, &cth_tok_init(string, line, offset), sizeof(cth_tokT));
        break;
    }
    code += len;
    offset += len;
  }
}

static cth_outputT cth_tokenise(
  const char* restrict code, const char* const restrict path, const size_t s_path,
  cth_variable_ctxT** const restrict variables, cth_error_ctxT* const restrict err
){
  rcT* const restrict rc_path = rc_make(path);
  cth_tokeniserT tokeniser; cth_tokeniser_init(&tokeniser);

// such descriptive names, clearly indicative of a cnile...
// C's only missing a defer{} statement (that doesn't require #include "stddefer.h" or some bs like that) & it'd be my favourite language.
// really it's missing only a handful of features to make it top1, but adding them would break backwards compat...
// one should make their own C standard that breaks backwards compat somewhat at the expense of a better experience.
//  or just use zig... but it isn't stable yet & I want my stuff to compile for the next thousand years.
// myC would have all features zig does, basically native C interop (not 100% backwards compatibility though) & more.
// I wouldn't like it if it didn't have dependent types, though I'd barely use them.
  cth_tokenise__single(code, rc_path, s_path, err, &tokeniser);
  cth_outputT tokenised = cth_tokeniser_destroy(&tokeniser, variables, err, rc_path);
  rc_disown(rc_path);
// result of tokenisation.
  return tokenised;
}

#endif
