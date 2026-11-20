#ifndef CTH_TOKENISER_OPS_IF_H_INCLUDED
#define CTH_TOKENISER_OPS_IF_H_INCLUDED

// handles if, ternary-if operators & statements.

#include "../ctx.h"             // basic ctx stuff.

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
  cth_tokT top = CTH_TOK__NONE;
  if(!vfastarr_top(tokeniser->ops, &top, sizeof(top)) || top != required_prev){
    printf("top is %u, size of ops is %lu.\n", top, vfastarr_size(tokeniser->ops));
    const cth_error_typeT err_type = tok == CTH_TOK__ELSE? CTH_ERROR__IF_ELSE_NO_PREV: CTH_ERROR__IF_TERNARY_ELSE_NO_PREV;
    cth_error_report(err_type, CTH_ERROR__COMPILER, path, tokeniser->line, tokeniser->offset, err);
    return;
  }
  vfastarr_remove(tokeniser->ops, sizeof(*tokeniser->ops));
// CTH_TOK__IF, CTH_TOK__QUESTION behave the exact same in operations. we use CTH_TOK__IF for both in the AST node.
  cth_astT* node = cth_ast_init(CTH_TOK__IF, tokeniser->line, tokeniser->offset, CTH_AST__IF);
  cth_nameT id_failure, id_success, id_condition;
  if(
    !vfastarr_pop(tokeniser->values, &id_failure, sizeof(id_failure)) ||
    !vfastarr_pop(tokeniser->values, &id_success, sizeof(id_success)) ||
    !vfastarr_pop(tokeniser->values, &id_condition, sizeof(id_condition))
  ){
    cth_error_report(CTH_ERROR__EXPECTED_EXPRESSION, CTH_ERROR__COMPILER, path, tokeniser->line, tokeniser->offset, err);
    return;
  }

  cth_astT *const failure = cth_ast_make_id(id_failure, tokeniser->line, tokeniser->offset),
           *const success = cth_ast_make_id(id_success, tokeniser->line, tokeniser->offset),
           *const condition = cth_ast_make_id(id_condition, tokeniser->line, tokeniser->offset);

  node->data._if.failure = vfastarr_append(node->data._if.failure, &failure, sizeof(failure));
  node->data._if.success = vfastarr_append(node->data._if.success, &success, sizeof(success));
  node->data._if.condition = condition;

  tokeniser->values = vfastarr_append(tokeniser->values, &node, sizeof(node));
}

#endif
