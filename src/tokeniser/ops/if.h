#ifndef CTH_TOKENISER_OPS_IF_H_INCLUDED
#define CTH_TOKENISER_OPS_IF_H_INCLUDED

// handles if, ternary-if operators & statements.

#include "_includes.h"

// invoked possibly due to an invalid construction of an if or of a ternary if statemet.
static void cth_tokeniser_op__if_error(
  const cth_tokT tok, cth_tokeniserT* const restrict tokeniser,
  rcT* const restrict path, cth_error_ctxT* const restrict err
){
  cth_error_report(
    tok == CTH_TOK__IF? CTH_ERROR__IF_WRONG: CTH_ERROR__IF_TERNARY_WRONG,
    CTH_ERROR__COMPILER,
    path, tokeniser->line, tokeniser->offset, err
  );
}

// handles if in its operator form & ternary ifs. 
static void cth_tokeniser_op__if(
  const cth_tokT tok, cth_tokeniserT* const restrict tokeniser, 
  rcT* const restrict path, cth_error_ctxT* const restrict err
){
  const cth_tokT required_prev = tok == CTH_TOK__ELSE? CTH_TOK__IF: CTH_TOK__QUESTION;
  const cth_tokT top = cth_tokeniser_scope_ops_top(&tokeniser->scope);
  if(top == CTH_TOK__NONE || top != required_prev){
    const cth_error_typeT err_type = tok == CTH_TOK__ELSE? CTH_ERROR__IF_ELSE_NO_PREV: CTH_ERROR__IF_TERNARY_ELSE_NO_PREV;
    cth_error_report(err_type, CTH_ERROR__COMPILER, path, tokeniser->line, tokeniser->offset, err);
    return;
  }
  cth_tokeniser_scope_ops_remove(&tokeniser->scope, 1);
// CTH_TOK__IF, CTH_TOK__QUESTION behave the exact same in operations. we use CTH_TOK__IF for both in the AST node.
  cth_astT* node = cth_ast_init(CTH_TOK__IF, tokeniser->line, tokeniser->offset, CTH_AST__IF);
  cth_nameT id_failure, id_success, id_condition;
  if(
    !(id_failure = cth_tokeniser_scope_values_pull(&tokeniser->scope)) ||
    !(id_success = cth_tokeniser_scope_values_pull(&tokeniser->scope)) ||
    !(id_condition = cth_tokeniser_scope_values_pull(&tokeniser->scope))
  ){
    cth_error_report(CTH_ERROR__EXPECTED_EXPRESSION, CTH_ERROR__COMPILER, path, tokeniser->line, tokeniser->offset, err);
    return;
  }

  cth_astT *const failure = cth_ast_make_id(id_failure, tokeniser->line, tokeniser->offset),
           *const success = cth_ast_make_id(id_success, tokeniser->line, tokeniser->offset),
           *const condition = cth_ast_make_id(id_condition, tokeniser->line, tokeniser->offset);

  node->data._if.failure = vfastarr_append(node->data._if.failure, &failure, sizeof failure);
  node->data._if.success = vfastarr_append(node->data._if.success, &success, sizeof success);
  node->data._if.condition = condition;

  cth_tokeniser_scope_values_push(&tokeniser->scope, (cth_nameT)node);
}

// tokenises if statement/code correctly.
static const char* cth_tokenise__if(
  cth_tokeniserT* const restrict tokeniser, const char* restrict code,
  rcT* const restrict path, cth_error_ctxT* const restrict err
){
  {
    cth_tokT tok;
    do{
      size_t len;
      tok = cth_tok(code, &len);
      if(
        tok == CTH_TOK__OPENED_SINGLE_LINE_COMMENT ||
        tok == CTH_TOK__OPENED_MULTI_LINE_COMMENT ||
        tok == CTH_TOK__OPENED_ROUND_BRACKET ||
        tok == CTH_TOK__WHITESPACE ||
        tok == CTH_TOK__NEWLINE
      )
        code = cth_tokeniser_dispatch(code, tok, len, tokeniser, path, err);
      else{
        cth_error_report(CTH_ERROR__IF_NO_ROUND_BRACKET, CTH_ERROR__COMPILER, path, tokeniser->line, tokeniser->offset, err);
        return code + len;
      }
    } while(tok != CTH_TOK__OPENED_ROUND_BRACKET);
  }
  {
    signed depth = 1;
    do{
      size_t len;
      const cth_tokT tok = cth_tok(code, &len);
      if(tok == CTH_TOK__OPENED_ROUND_BRACKET) depth++; else
      if(tok == CTH_TOK__CLOSED_ROUND_BRACKET) depth--;
      code = cth_tokeniser_dispatch(code, tok, len, tokeniser, path, err);
    } while(depth);
  }
  cth_tokT _if = CTH_TOK__IF;
  cth_tokeniser_scope_ops_push(&tokeniser->scope, (cth_nameT)_if);
  tokeniser->e_state = tokeniser->e_state & ~CTH_TOKENISER__PREV_WAS_NAME | CTH_TOKENISER__UNARY_ALLOWED;

  return code;
}

#endif
