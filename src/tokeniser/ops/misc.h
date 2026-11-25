#ifndef CTH_TOKENISER_OPS_MISC_H_INCLUDED
#define CTH_TOKENISER_OPS_MISC_H_INCLUDED

#include "_includes.h"

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
  cth_tokeniser_scope_inc(&tokeniser->scope);
  return code + len;
}

static const char* cth_tokeniser_op__closed_curly_bracket(
  cth_tokeniserT* const restrict tokeniser, const char* const restrict code, const size_t len,
  rcT* const restrict path, cth_error_ctxT* const restrict err
){
  if(!cth_tokeniser_scope_dec(&tokeniser->scope, tokeniser, path, err))
    cth_error_report(CTH_ERROR__INDENTATION_DEPTH_NEGATIVE, CTH_ERROR__COMPILER, path, tokeniser->line, tokeniser->offset, err);
  return code + len;
}

static const char* cth_tokeniser_op__opened_round_bracket(
  cth_tokeniserT* const restrict tokeniser, const char* const restrict code, const size_t len,
  rcT* const restrict path, cth_error_ctxT* const restrict err
){
  cth_tokeniser_scope_state_andor(&tokeniser->scope, ~CTH_TOKENISER__PREV_WAS_NAME, CTH_TOKENISER__UNARY_ALLOWED);
  cth_tokT opened = CTH_TOK__OPENED_ROUND_BRACKET;
  cth_tokeniser_scope_ops_push(&tokeniser->scope, opened);
  return code + len;
}

static const char* cth_tokeniser_op__closed_round_bracket(
  cth_tokeniserT* const restrict tokeniser, const char* const restrict code, const size_t len,
  rcT* const restrict path, cth_error_ctxT* const restrict err
){
  cth_tokeniser_scope_state_and(&tokeniser->scope, ~CTH_TOKENISER__UNARY_ALLOWED);
  while(1){
    const cth_tokT tok = cth_tokeniser_scope_ops_pull(&tokeniser->scope);
    if(tok == CTH_TOK__NONE){
      cth_error_report(CTH_ERROR__CLOSED_ROUND_BRACKET, CTH_ERROR__COMPILER, path, tokeniser->line, tokeniser->offset, err);
      break;
    }
    if(tok == CTH_TOK__OPENED_ROUND_BRACKET) break;
    cth_tokeniser_dispatch__ast(tok, tokeniser, path, err);
  }
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

// appends whatever value still present to program.
static void cth_tokeniser_op__semicolon(cth_tokeniserT* const restrict tokeniser){
// all persistent tokeniser state is cleared.
  const size_t c_scopes = cth_tokeniser_scope_countof(&tokeniser->scope);
  cth_tokeniser_scope__entryT* const level = &((cth_tokeniser_scope__entryT*)tokeniser->scope.internal.arr)[c_scopes - 1];
  const size_t c_values = vfastarr_elcnt(level->values, sizeof *level->values);

  for(size_t i = 0; i < c_values; i++){
    cth_astT* const node = cth_ast_make_id(level->values[i], tokeniser->line, tokeniser->offset);
    level->program->data.program.program = vfastarr_append(
      level->program->data.program.program,
      &node,
      sizeof *level->program->data.program.program
    );
  }

  vfastarr_empty(level->ops);
  vfastarr_empty(level->values);
  cth_tokeniser_scope_state_set(&tokeniser->scope, CTH_TOKENISER_STATE_INIT);
}

#endif
