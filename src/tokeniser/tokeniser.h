#ifndef SATISFIABLE_TOKENISER_H_INCLUDED
#define SATISFIABLE_TOKENISER_H_INCLUDED

#include "name.h"                   // name handling.
#include "../common/_common.h"      // tokens et cetera.

// stdlib includes.
#include "stddef.h"                 // for size_t.
#include "string.h"                 // for string manipulation functions.

typedef struct{
  satisfiable_tokT* arr;
  satisfiable_name_ctxT name;
  vfastbst_rcT files;
} satisfiable_tokeniserT;

static void satisfiable_tokeniser_init(satisfiable_tokeniserT* const restrict tokeniser){
  satisfiable_name_ctx_init(&tokeniser->name);
  tokeniser->arr = vfastarr_init(sizeof(*tokeniser->arr));
  tokeniser->files = VFASTBST_RC_INIT;
}

static satisfiable_outputT satisfiable_tokeniser_destroy(
  satisfiable_tokeniserT* const restrict tokeniser,  satisfiable_variable_ctxT** variables,
  satisfiable_error_ctxT* const restrict err, rcT* const restrict path
){
  if(variables) *variables = tokeniser->name.variables;
  else satisfiable_variable_ctx_destroy(tokeniser->name.variables, true);

  if(!satisfiable_name_ctx_destroy(&tokeniser->name, false))
    satisfiable_error_report(SATISFIABLE_ERROR__INDENTATION_DEPTH_NEGATIVE, SATISFIABLE_ERROR__COMPILER_DESTRUCTION, path, 0, 0, err);
  vfastbst_rc_destroy(&tokeniser->files);
  return (satisfiable_outputT)tokeniser->arr;
}

// path   ~ file path itself.
// s_path ~ length of path excluding actual filename.
// err    ~ error ctx struct holding all reported during compilation.
static void satisfiable_tokenise__single(
  const char* restrict code,
  rcT* const restrict path, const size_t s_path,
  satisfiable_error_ctxT* const restrict err,
  satisfiable_tokeniserT* const restrict ctx
);

// handles 'include' commands by parsing the path & pushing it to the queue, if it wasn't already.
static const char* satisfiable_tokenise__include(
  const char* restrict code, rcT* const restrict path, const size_t s_path,
  satisfiable_tokeniserT* const restrict tokeniser,
  satisfiable_error_ctxT* const restrict err, satisfiable_fposT* const restrict line, satisfiable_fposT* const restrict offset
){
  code += strcspn(code, "\"");
  if(code[0] != '"'){     // Include isn't followed by single quotes, so errors out.
    satisfiable_error_report(SATISFIABLE_ERROR__INCLUDE_NO_LEADING_QUOTES, SATISFIABLE_ERROR__COMPILER, path, *line, *offset, err);
    return code;
  }

  const size_t s_finalpath = strcspn(code, "\""); 
  code += s_finalpath;
  if(code[0] != '"'){
    satisfiable_error_report(SATISFIABLE_ERROR__INCLUDE_NO_LEADING_QUOTES, SATISFIABLE_ERROR__COMPILER, path, *line, *offset, err);
    return code;
  }

  rcT* const restrict finalpath = rc_make(vfastpath_append(path->str, s_path, code, s_finalpath));
  size_t s_filename;
  char* filename = strrchr(finalpath->str, '/');
  if(!filename) s_filename = 0;
  else s_filename = s_finalpath - (filename - finalpath->str) - 1 /* correction term, because we want to exclude last '/'. */;

  if((s_finalpath - s_filename) == s_path && !strcmp(path->str, finalpath->str)){
    satisfiable_error_report(SATISFIABLE_ERROR__INCLUDE_SELF_REFERENCE, SATISFIABLE_ERROR__COMPILER, path, *line, *offset, err);
    rc_disown(finalpath);
    return code;
  }

  vfastbst_rc_nodeT** const restrict success = vfastbst_rc_put_half(&tokeniser->files, finalpath);
// error out if an include's argument is ever NULL.
  satisfiable_tokT* new = NULL;
  if(!*success){
    const satisfiable__fileT finalcode = satisfiable_load__map(finalpath->str, true);
    if(!finalcode.code){
      satisfiable_error_report__str(SATISFIABLE_ERROR__INCLUDE_UNKNOWN_FILE, SATISFIABLE_ERROR__COMPILER, path, *line, *offset, err, finalpath->str);
      rc_disown(finalpath);
      return code;
    }

    void* const restrict old = tokeniser->arr;
    tokeniser->arr = vfastarr_init(sizeof(*tokeniser->arr));
    satisfiable_tokenise__single(finalcode.code, finalpath, s_finalpath - s_filename, err, tokeniser);
    satisfiable_load__unmap(finalcode);

    new = tokeniser->arr; tokeniser->arr = old;
    *success = vfastbst_rc_node_init(finalpath, (satisfiable__representation_tokT)new);
  }
  else
    new = (satisfiable_tokT*)(*success)->obj;

// when processing stuff, the 'Include' token is followed by some ptr to data representing the file being included, whatever that is.
  const satisfiable_tokT info = satisfiable_tok_init((satisfiable__underlying_tokT)new, *line, *offset);
  tokeniser->arr = vfastarr_append(tokeniser->arr, &info, sizeof(info));

  rc_disown(finalpath);
  return code;
}

static const char* satisfiable_tokenise__comment__single(
  const char* restrict code,
  satisfiable_fposT* const restrict line,
  satisfiable_fposT* const restrict offset
){
  const char* restrict n_code = strchr(code, '\n');
  if(!n_code) n_code = code + strlen(code);                     // I'd use strchrnul but it's GNU only.
  else n_code++;                                                // this way it points after the '\n'.
  (*line)++;
  *offset += code - n_code;
  return n_code;
}

static const char* satisfiable_tokenise__comment__multi(
  const char* restrict code,
  satisfiable_fposT* const restrict line,
  satisfiable_fposT* const restrict offset
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
static void satisfiable_tokenise__single(
  const char* restrict code, rcT* const restrict path, const size_t s_path,
  satisfiable_error_ctxT* const restrict err, satisfiable_tokeniserT* const restrict tokeniser
){
  satisfiable_fposT line = 0, offset = 0;
  satisfiable_tokT tok = satisfiable_tok_init(SATISFIABLE_TOK__SPECIAL_ERROR, line, offset);

// ADD SPECIAL HANDLING FOR:
//  names.

  do{
    size_t len;
    tok = satisfiable_tok(code, &len, line, offset);
// tokens that should be ignored.
    switch(tok.token){
      case SATISFIABLE_TOK__NEWLINE:
        line++; offset = 0;
        code += len;
        continue;
      case SATISFIABLE_TOK__WHITESPACE:
        offset += len;
        code += len;
        continue;
      case SATISFIABLE_TOK__OPENED_SINGLE_LINE_COMMENT:
        code = satisfiable_tokenise__comment__single(code + len, &line, &offset);
        continue;
      case SATISFIABLE_TOK__OPENED_MULTI_LINE_COMMENT:
        code = satisfiable_tokenise__comment__multi(code + len, &line, &offset);
        continue;
      case SATISFIABLE_TOK__NAME:
        const satisfiable_nameT name = satisfiable_name_add(code, len, err, &tokeniser->name);
        tokeniser->arr = vfastarr_append(
          tokeniser->arr,
          &satisfiable_tok_init__macro(name, line, offset),
          sizeof(satisfiable_tokT)
        );
        offset += len; code += len;
        continue;
      case SATISFIABLE_TOK__VALUE:
        const satisfiable_nameT value = satisfiable_value_add(code, len, &tokeniser->name, err, path, line, offset);
        tokeniser->arr = vfastarr_append(
          tokeniser->arr,
          &satisfiable_tok_init__macro(value, line, offset),
          sizeof(satisfiable_tokT)
        );
        offset += len; code += len;
        continue;
      case SATISFIABLE_TOK__SPECIAL_IMPOSSIBILITY:
        satisfiable_error_report__u32(SATISFIABLE_ERROR__IMPOSSIBILITY, SATISFIABLE_ERROR__COMPILER, path, line, offset, err, SATISFIABLE_IMPOSSIBILITY__TOK_H_RETURN_VALUE);
        offset += len; code += len;
        continue;
      default:
        break;
    }
// name handling:
//  try to allocate name.
    tokeniser->arr = vfastarr_append(tokeniser->arr, &tok, sizeof(tok));
// special handling of certain tokens.
    switch(tok.token){
      case SATISFIABLE_TOK__INCLUDE:
        code = satisfiable_tokenise__include(code, path, s_path, tokeniser, err, &line, &offset);
        break;
      case SATISFIABLE_TOK__OPENED_CURLY_BRACKET:
        satisfiable_name_depth_inc(&tokeniser->name);
        break;
      case SATISFIABLE_TOK__CLOSED_CURLY_BRACKET:
        if(!satisfiable_name_depth_dec(&tokeniser->name))
          satisfiable_error_report(SATISFIABLE_ERROR__INDENTATION_DEPTH_NEGATIVE, SATISFIABLE_ERROR__COMPILER, path, line, offset, err);
        break;
      default:
        break;
    }
    code += len;
    offset += len;
  } while(tok.token != SATISFIABLE_TOK__EOF);
}

static satisfiable_outputT satisfiable_tokenise(
  const char* restrict code, const char* const restrict path, const size_t s_path,
  satisfiable_variable_ctxT** const restrict variables, satisfiable_error_ctxT* const restrict err
){
  rcT* const restrict rc_path = rc_make(path);
  satisfiable_tokeniserT tokeniser; satisfiable_tokeniser_init(&tokeniser);

// such descriptive names, clearly indicative of a cnile...
// C's only missing a defer{} statement (that doesn't require #include "stddefer.h" or some bs like that) & it'd be my favourite language.
// really it's missing only a handful of features to make it top1, but adding them would break backwards compat...
// one should make their own C standard that breaks backwards compat somewhat at the expense of a better experience.
//  or just use zig... but it isn't stable yet & I want my stuff to compile for the next thousand years.
// myC would have all features zig does, basically native C interop (not 100% backwards compatibility though) & more.
// I wouldn't like it if it didn't have dependent types, though I'd barely use them.
  satisfiable_tokenise__single(code, rc_path, s_path, err, &tokeniser);
  satisfiable_outputT tokenised = satisfiable_tokeniser_destroy(&tokeniser, variables, err, rc_path);
  rc_disown(rc_path);
// result of tokenisation.
  return tokenised;
}

#endif
