#ifndef CTH_TOKENISER_OPS_BINARY_H_INCLUDED
#define CTH_TOKENISER_OPS_BINARY_H_INCLUDED

#include "../ctx.h"

static cth_astT* cth_tokeniser_op__get_node(cth_tokeniserT* const restrict tokeniser, rcT* const restrict path, cth_error_ctxT* const restrict err){
  cth_nameT id_src;
  if(!vfastarr_pop(tokeniser->values, &id_src, sizeof(id_src))){
    const size_t top = vfastarr_elcnt(tokeniser->program->data.program.program, sizeof(cth_astT*));
    tokeniser->program->data.program.program = vfastarr_extend(tokeniser->program->data.program.program, 1, sizeof(cth_astT*));
    cth_ast_init(&tokeniser->program->data.program.program[top], 0, tokeniser->line, tokeniser->offset, CTH_AST__ID);
  // reports that there aren't enough operands for this.
    cth_error_report(CTH_ERROR__AST_TOO_FEW_OPERANDS, CTH_ERROR__COMPILER, path, tokeniser->line, tokeniser->offset, err);
    return NULL;
  }

  cth_astT* src;
  if(id_src & 1){
    src = cth_ast_init(NULL, CTH_TOK__NAME, tokeniser->line, tokeniser->offset, CTH_AST__ID);
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
// we add to src->up always and we modify the upper node appropriately.
  cth_astT** restrict reference;
  if(!src->reference){
    const size_t top = vfastarr_elcnt(tokeniser->program->data.program.program, sizeof(cth_astT*));
    tokeniser->program->data.program.program = vfastarr_extend(tokeniser->program->data.program.program, 1, sizeof(cth_astT*));
    reference = &tokeniser->program->data.program.program[top];
  }
  else reference = src->reference;

  cth_astT* node = cth_ast_init(reference, tok, tokeniser->line, tokeniser->offset, CTH_AST__UNARY);

  node->data._unary.src = src;
  src->reference = &node->data._unary.src;

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
  cth_astT** restrict reference;
  if(!src0->reference && !src1->reference){
    const size_t top = vfastarr_elcnt(tokeniser->program->data.program.program, sizeof(cth_astT*));
    tokeniser->program->data.program.program = vfastarr_extend(tokeniser->program->data.program.program, 1, sizeof(cth_astT*));
    reference = &tokeniser->program->data.program.program[top];
  }
  else
    reference = src0->reference? src0->reference: src1->reference;

  cth_astT* node = cth_ast_init(reference, tok, tokeniser->line, tokeniser->offset, CTH_AST__BINARY);

  node->data._binary.s0 = src0;
  node->data._binary.s1 = src1;
  src0->reference = &node->data._binary.s0;
  src1->reference = &node->data._binary.s1;

  tokeniser->values = vfastarr_append(tokeniser->values, (void*)&node, sizeof(cth_nameT));
}


#endif
