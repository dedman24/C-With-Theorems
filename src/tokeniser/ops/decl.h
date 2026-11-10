#ifndef CTH_TOKENISER_OPS_DECL_H_INCLUDED
#define CTH_TOKENISER_OPS_DECL_H_INCLUDED

#include "../ctx.h"

// declaration handling was a mess; this function streamlines all of it.

static cth_nameT cth_tokeniser_op__name(
  cth_tokeniserT* const restrict tokeniser, const char* const restrict code, const size_t len,
  rcT* const restrict path, cth_error_ctxT* const restrict err
);

// handles declarations & their syntax.
static const char* cth_tokeniser_op__declaration(
  cth_tokeniserT* const restrict tokeniser, const char* restrict code,
  rcT* const restrict path, cth_error_ctxT* const restrict err
){
  if(tokeniser->state & CTH_TOKENISER__DECLARATION){
    cth_error_report(CTH_ERROR__DECLARATION_RECURSIVE, CTH_ERROR__COMPILER, path, tokeniser->line, tokeniser->offset, err);
    return code + strcspn(code, ";");
  }

// parse type:
//  push low-prio token.
//  check that there's only 1tok & 0ops, error out otherwise.
//  ast_node :: cth_astT* = switch value~top() & 1
//    0 -> value~top(),
//    1 -> make_ast(value~top());
// parse name, put in proper position.
// take token; all must be whitespace || comments.
//  repeat until we reach CTH_TOK__COMMA, CTH_TOK__SEMICOLON or CTH_TOK__ASSIGNMENT.
// if(tok == CTH_TOK__ASSIGNMENT) all following is expression for assignment; handle it until COMMA or SEMICOLON is reached.
  cth_tokeniser_dispatch(NULL, CTH_TOK__DECLARATION, 0, tokeniser, path, err);

  const size_t s_values = vfastarr_size(tokeniser->values), s_ops = vfastarr_size(tokeniser->ops);
  if(s_values != sizeof(*tokeniser->values) || s_ops != 0){
    cth_error_report(CTH_ERROR__DECLARATION_WRONG, CTH_ERROR__COMPILER, path, tokeniser->line, tokeniser->offset, err);
    return code + strcspn(code, ";");
  }

  const size_t c_program = vfastarr_elcnt(tokeniser->program->data.program.program, sizeof(*tokeniser->program->data.program.program));
  cth_astT* const restrict decl = cth_ast_init(
    &tokeniser->program->data.program.program[c_program],
    CTH_TOK__DECLARATION,
    tokeniser->line, tokeniser->offset,
    CTH_AST__DECLARATION
  );

  cth_nameT id_type; vfastarr_pop(tokeniser->values, &id_type, sizeof(id_type));
  if(id_type & 1){
    cth_astT* const restrict ast_type = cth_ast_init(&decl->data._declaration.type, CTH_TOK__NAME, tokeniser->line, tokeniser->offset, CTH_AST__ID);
    ast_type->data.id = id_type;
  }
  else{
    cth_astT* const restrict ast_type = (cth_astT*)id_type;
    ast_type->reference = &decl->data._declaration.type;
  }
// modelled as a state machine.
// diagram:
//  NAME --CTH_TOK__NAME-> ASSIGN --CTH_TOK__ASSIGNMENT-> EXPR --CTH_TOK__SEMICOLON-> QUIT
//   ^------------------- CTH_TOK__COMMA -------------------|
  enum{
    NAME,
    ASSIGN,
    EXPR,
    QUIT
  } state;
// clears PREV_WAS_NAME bit, sets DECLARATION bit.
  tokeniser->state = tokeniser->state & ~CTH_TOKENISER__PREV_WAS_NAME | CTH_TOKENISER__DECLARATION;
// parse everything until comma
  cth_tokT tok;
  do{
    size_t len;
    tok = cth_tok(code, &len);

    if(
      tok == CTH_TOK__OPENED_SINGLE_LINE_COMMENT ||
      tok == CTH_TOK__OPENED_MULTI_LINE_COMMENT ||
      tok == CTH_TOK__WHITESPACE ||
      tok == CTH_TOK__NEWLINE
    ){
      code = cth_tokeniser_dispatch(code, tok, len, tokeniser, path, err);
      continue;
    }

    if(state == NAME){
      state = ASSIGN;

      if(tok != CTH_TOK__NAME){
        cth_error_report(CTH_ERROR__DECLARATION_EXPECTED_ID, CTH_ERROR__COMPILER, path, tokeniser->line, tokeniser->offset, err);
        code += strcspn(code, ";");
        break;
      }

      const cth_nameT id = cth_tokeniser_op__name(tokeniser, code, len, path, err);
      if(!id){ code += strcspn(code, ";"); break; }
      decl->data._declaration.vars = vfastarr_append(decl->data._declaration.vars, &id, sizeof(id));
      code += len;
    }
    else if(state == ASSIGN){
      state = EXPR;

      if(tok != CTH_TOK__ASSIGNMENT){
        cth_error_report(CTH_ERROR__DECLARATION_EXPECTED_ID, CTH_ERROR__COMPILER, path, tokeniser->line, tokeniser->offset, err);
        code += strcspn(code, ";");
        break;
      }
      code += len;
    }
    else if(state == EXPR){
// TODO: fix this.
      if(tok == CTH_TOK__SEMICOLON){ code += len; break; }
      if(tok == CTH_TOK__COMMA){ code += len; state = NAME; }
      else code = cth_tokeniser_dispatch(code, tok, len, tokeniser, path, err);
    }
  } while(1);

  tokeniser->state &= ~CTH_TOKENISER__DECLARATION;
  return code;
}


#endif
