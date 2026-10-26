#ifndef CTH_TOKENISER_OPS_INCLUDED
#define CTH_TOKENISER_OPS_INCLUDED

#include "../ctx.h"

#include "misc.h"
#include "arity.h"
#include "names.h"

// generates AST node holding 'tok'.
static void cth_tokeniser_dispatch__ast(
  const cth_tokT tok, cth_tokeniserT* const restrict tokeniser, 
  rcT* const restrict path, cth_error_ctxT* const restrict err
){
  if(tok == CTH_TOK__IF);
  else if(tok == CTH_TOK__FOR);
  else if(tok == CTH_TOK__DEFER);
  else if(tok == CTH_TOK__DO);
  else if(tok == CTH_TOK__WHILE);
  else if(tok == CTH_TOK__SWITCH);
  else if(tokeniser->state & CTH_TOKENISER__PREV_WAS_NAME)
    cth_tokeniser_op__binary(tok, tokeniser, path, err);
  else
    cth_tokeniser_op__unary(tok, tokeniser, path, err);
}

static const char* cth_tokeniser_dispatch(
  const char* restrict code, const cth_tokT tok, const size_t len, cth_tokeniserT* const restrict tokeniser, 
  rcT* const restrict path, cth_error_ctxT* const restrict err
){
// special cases to be handled on their own.
  switch(tok){
    case CTH_TOK__OPENED_SINGLE_LINE_COMMENT: return cth_tokeniser_op__comment_single(tokeniser, code, len, path, err);
    case CTH_TOK__OPENED_MULTI_LINE_COMMENT: return cth_tokeniser_op__comment_multi(tokeniser, code, len, path, err);
    case CTH_TOK__OPENED_CURLY_BRACKET: return cth_tokeniser_op__opened_curly_bracket(tokeniser, code, len, path, err);
    case CTH_TOK__CLOSED_CURLY_BRACKET: return cth_tokeniser_op__closed_curly_bracket(tokeniser, code, len, path, err);
    case CTH_TOK__WHITESPACE: return cth_tokeniser_op__whitespace(tokeniser, code, len, path, err);
    case CTH_TOK__NEWLINE: return cth_tokeniser_op__newline(tokeniser, code, len, path, err);
    case CTH_TOK__STRING: return cth_tokeniser_op__string(tokeniser, code, len, path, err);
    case CTH_TOK__VALUE: return cth_tokeniser_op__value(tokeniser, code, len, path, err);
    case CTH_TOK__CHAR: return cth_tokeniser_op__char(tokeniser, code, len, path, err);
    case CTH_TOK__NAME: return cth_tokeniser_op__name(tokeniser, code, len, path, err);
    case CTH_TOK__NONE:
      cth_error_report(CTH_ERROR__TOKENISER_UNKNOWN_TOKEN, CTH_ERROR__COMPILER, path, tokeniser->line, tokeniser->offset, err);
      return code + len;
    default: break;
  }

  const size_t cnt_top = vfastarr_elcnt(tokeniser->ops, sizeof(*tokeniser->ops));

  if(cnt_top){
    cth_tokT top = tokeniser->ops[cnt_top - 1];
    if(cth_priority[top] <= cth_priority[tok])
      goto SKIP_PRIO;
    vfastarr_remove(tokeniser->ops, sizeof(*tokeniser->ops));
    do{
      cth_tokeniser_dispatch__ast(top, tokeniser, path, err);
      if(!vfastarr_pop(tokeniser->ops, &top, sizeof(top)))
        break;
    } while(cth_priority[top] > cth_priority[tok]);
  }
SKIP_PRIO:
  tokeniser->state &= ~CTH_TOKENISER__PREV_WAS_NAME;
  if(tok != CTH_TOK__SEMICOLON)
    vfastarr_append(tokeniser->ops, &tok, sizeof(tok));

  return code + len;
}

#endif
