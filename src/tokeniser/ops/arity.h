#ifndef CTH_TOKENISER_OPS_BINARY_H_INCLUDED
#define CTH_TOKENISER_OPS_BINARY_H_INCLUDED

#include "../ctx.h"

// sees if token has corresponding unary version, returns CTH_TOK__NONE if none is found.
static cth_tokT cth_tokeniser_unary_tok(const cth_tokT op){
  switch(op){
    case CTH_TOK__PLUS: return CTH_TOK__UNARY_PLUS;
    case CTH_TOK__MINUS: return CTH_TOK__UNARY_MINUS;
    case CTH_TOK__STAR: return CTH_TOK__UNARY_STAR;
    case CTH_TOK__AMPERSAND: return CTH_TOK__UNARY_AMPERSAND;
    default: return CTH_TOK__NONE;
  }
}

static cth_astT* cth_tokeniser_op__get_node(cth_tokeniserT* const restrict tokeniser, rcT* const restrict path, cth_error_ctxT* const restrict err){
  cth_nameT id_src;
  if(!vfastarr_pop(tokeniser->values, &id_src, sizeof(id_src))){      // not enough operands.
    cth_astT* node = cth_ast_init(CTH_TOK__NAME, tokeniser->line, tokeniser->offset, CTH_AST__ID);
    tokeniser->values = vfastarr_append(tokeniser->values, &node, sizeof(node)); 
    cth_error_report(CTH_ERROR__AST_TOO_FEW_OPERANDS, CTH_ERROR__COMPILER, path, tokeniser->line, tokeniser->offset, err);
    return NULL;
  }

  if(id_src & 1){
    cth_astT* const restrict src = cth_ast_init(CTH_TOK__NAME, tokeniser->line, tokeniser->offset, CTH_AST__ID);
    src->data.id = id_src;
    return src;
  }
  return (cth_astT*)id_src;
}

// generates AST node holding 'tok' as a unary operation.
static void cth_tokeniser_op__unary(
  const cth_tokT tok, cth_tokeniserT* const restrict tokeniser, 
  rcT* const restrict path, cth_error_ctxT* const restrict err
){
  cth_astT* const restrict src = cth_tokeniser_op__get_node(tokeniser, path, err);
  if(!src) return;
  cth_astT* node = cth_ast_init(tok, tokeniser->line, tokeniser->offset, CTH_AST__UNARY);
  node->data._unary.src = src;
  tokeniser->values = vfastarr_append(tokeniser->values, (void*)&node, sizeof(cth_nameT));
}

// generates AST node holding 'tok' as a binary operation.
static void cth_tokeniser_op__binary(
  const cth_tokT tok, cth_tokeniserT* const restrict tokeniser, 
  rcT* const restrict path, cth_error_ctxT* const restrict err
){
  cth_astT *const restrict src0 = cth_tokeniser_op__get_node(tokeniser, path, err),
           *const restrict src1 = cth_tokeniser_op__get_node(tokeniser, path, err);
  if(!src0 || !src1) return;

// we add to src->up always and we modify the upper node appropriately.
  cth_astT* node = cth_ast_init(tok, tokeniser->line, tokeniser->offset, CTH_AST__BINARY);
  node->data._binary.s0 = src0;
  node->data._binary.s1 = src1;
  tokeniser->values = vfastarr_append(tokeniser->values, (void*)&node, sizeof(cth_nameT));
}


#endif
