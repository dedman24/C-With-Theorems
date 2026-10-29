#ifndef CTH_TOKENISER_OPS_MISC_H_INCLUDED
#define CTH_TOKENISER_OPS_MISC_H_INCLUDED

#include "../ctx.h"             // tokeniserT.

static const char* cth_tokeniser_op__whitespace(
  cth_tokeniserT* const restrict tokeniser, const char* const restrict code, const size_t len,
  rcT* const restrict path, cth_error_ctxT* const restrict err
){
  return code + len;
}

static const char* cth_tokeniser_op__newline(
  cth_tokeniserT* const restrict tokeniser, const char* const restrict code, const size_t len,
  rcT* const restrict path, cth_error_ctxT* const restrict err
){
  tokeniser->offset = 0; tokeniser->line++;
  return code + len;
}

static const char* cth_tokeniser_op__opened_curly_bracket(
  cth_tokeniserT* const restrict tokeniser, const char* const restrict code, const size_t len,
  rcT* const restrict path, cth_error_ctxT* const restrict err
){
  cth_name_depth_inc(&tokeniser->name);
  return code + len;
}

static const char* cth_tokeniser_op__closed_curly_bracket(
  cth_tokeniserT* const restrict tokeniser, const char* const restrict code, const size_t len,
  rcT* const restrict path, cth_error_ctxT* const restrict err
){
  if(!cth_name_depth_dec(&tokeniser->name))
    cth_error_report(CTH_ERROR__INDENTATION_DEPTH_NEGATIVE, CTH_ERROR__COMPILER, path, tokeniser->line, tokeniser->offset, err);
  return code + len;
}

static const char* cth_tokeniser_op__comment_single(
  cth_tokeniserT* const restrict tokeniser, const char* const restrict code, const size_t len,
  rcT* const restrict path, cth_error_ctxT* const restrict err
){
  const char* restrict n_code = strchr(code, '\n');
  if(!n_code) n_code = code + strlen(code);                     // I'd use strchrnul but it's GNU only.
  else n_code++;                                                // this way it points after the '\n'.
  tokeniser->offset = 0; tokeniser->line++;
  return n_code;
}

static const char* cth_tokeniser_op__comment_multi(
  cth_tokeniserT* const restrict tokeniser, const char* restrict code, const size_t len,
  rcT* const restrict path, cth_error_ctxT* const restrict err
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
      tokeniser->line += 1; tokeniser->offset = 0;
      code = newln + 1;
      newln = strchr(code, '\n');
    }
    else if(
      closest_opened &&
      (closest_opened < newln || !newln) &&
      (closest_opened < closest_closed || !closest_closed)
    ){
      tokeniser->offset += closest_opened - code; depth++;
      code = closest_opened + 2 + 1;
      closest_opened = strstr(code, "/*");
    }
    else if(
      closest_closed &&
      (closest_closed < newln || !newln) &&
      (closest_closed < closest_opened || !closest_opened)
    ){
      tokeniser->offset += closest_closed - code; depth--;
      code = closest_closed + 2 + 1;
      closest_closed = strstr(code, "*/");
    }
  } while(depth && code[0]);
  return code;
}

static void cth_tokeniser_op__semicolon(cth_tokeniserT* const restrict tokeniser){
  const size_t c_elem = vfastarr_elcnt(tokeniser->values, sizeof(*tokeniser->values));
  for(size_t i = 0; i < c_elem; i++){
    const cth_nameT name = tokeniser->values[i];
    if(!(name & 1))
      break;
    tokeniser->program->data.program.program = vfastarr_extend(tokeniser->program->data.program.program, c_elem, sizeof(*tokeniser->program->data.program.program));
    cth_astT* const restrict node = cth_ast_init(&tokeniser->program->data.program.program[i], CTH_TOK__NAME, tokeniser->line, tokeniser->offset, CTH_AST__ID);
    node->data.id = name;
  }
  vfastarr_remove(tokeniser->values, c_elem*sizeof(*tokeniser->values));
}

#endif
