#ifndef CTH_TOKENISER_OPS_DECL_H_INCLUDED
#define CTH_TOKENISER_OPS_DECL_H_INCLUDED

#include "../ctx.h"
#include "qualifs.h"                                  // cth_tokeniser_op__qualifier.
#include "../../interpreter/ops/typecheck.h"          // typechecking.

// declaration handling was a mess; this function streamlines all of it.

static const char* cth_tokeniser_op__name(
  cth_tokeniserT* const restrict tokeniser, const char* const restrict code, const size_t len,
  rcT* const restrict path, cth_error_ctxT* const restrict err
);

struct cth_tokeniser_op__declaration_master_typeS{
  cth_nameT id;
  cth_variableT* var;
};

// obtains master type in declaration from variable holding normal type.
// a 'master type' is everything before a '*', '&', '[]' and so on; it is just the type that C extends to all members of a declaration.
// idk if this makes sense, it does in my head :PPP.
static struct cth_tokeniser_op__declaration_master_typeS cth_tokeniser_op__declaration_master_type(
  cth_tokeniserT* const restrict tokeniser, cth_variableT* const var
){
  cth_type__internalT* const c_internal = cth_type__internal_make_master(var->internal.type);

  const cth_nameT id_master = cth_variable_name(tokeniser->name.variables);
  cth_variableT* var_master = cth_variable_add(tokeniser->name.variables, id_master, NULL, 0, cth_typevar(CTH_TYPE__TYPE), CTH_VARSTATE__NONE);
  var_master->internal.type = c_internal;
  return (struct cth_tokeniser_op__declaration_master_typeS){ .id = id_master, .var = var_master };
}

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
// used to track current variable type.
  cth_nameT id_type; vfastarr_pop(tokeniser->values, &id_type, sizeof(id_type));
  cth_variableT* var_type = cth_variable_search(tokeniser->name.variables, id_type);
// used to track 'master' type.
  const struct cth_tokeniser_op__declaration_master_typeS master = cth_tokeniser_op__declaration_master_type(tokeniser, var_type);


  cth_astT* const decl = cth_ast_init(CTH_TOK__DECLARATION, tokeniser->line, tokeniser->offset, CTH_AST__DECLARATION);
// declarations are modelled as a state machine.
// diagram:
//  NAME --CTH_TOK__NAME-> ASSIGN --CTH_TOK__ASSIGNMENT-> EXPR --CTH_TOK__SEMICOLON-> QUIT
//   ^------------------- CTH_TOK__COMMA -----------------+-|
  enum{ NAME, ASSIGN, EXPR } state = NAME;
// clears PREV_WAS_NAME bit, sets DECLARATION bit.
  tokeniser->state = tokeniser->state & ~CTH_TOKENISER__PREV_WAS_NAME | CTH_TOKENISER__DECLARATION;
// parse everything until comma
  do{
    size_t len;
    const cth_tokT tok = cth_tok(code, &len);

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
// note: to support compound qualifiers, this might have to be changed.
      const struct cth_name_addS added = cth_name_add(code, len, CTH_TYPE__NONE, CTH_VARSTATE__NONE, &tokeniser->name);
      if(!added.var){
        if(!cth_qualifier_is__id(added.id)){
          cth_error_report__str_size(CTH_ERROR__DECLARATION_REDEFINITION, CTH_ERROR__COMPILER, path, tokeniser->line, tokeniser->offset, err, code, len);
          return code + strcspn(code, ";");
        }
        const struct cth_tokeniser_op__qualifierS qualif = cth_tokeniser_op__qualifier(tokeniser, added.id, id_type, var_type, true, path, err);
        id_type = qualif.id;
        var_type = qualif.var;
        continue;
      }

      added.var->type = var_type;
      decl->data._declaration.vars = vfastarr_append(decl->data._declaration.vars, &added.id, sizeof(added.id));
      
      cth_astT* const ast_type = cth_ast_init(CTH_TOK__NAME, tokeniser->line, tokeniser->offset, CTH_AST__ID);
      ast_type->data.id = id_type;
      decl->data._declaration.types = vfastarr_append(decl->data._declaration.types, &ast_type, sizeof(ast_type));
      var_type = master.var;
      id_type = master.id;

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
      if(tok != CTH_TOK__SEMICOLON && tok != CTH_TOK__COMMA){
        code = cth_tokeniser_dispatch(code, tok, len, tokeniser, path, err);
        continue;
      }
      cth_tokeniser_dispatch(NULL, CTH_TOK__DECLARATION, 0, tokeniser, path, err);
      cth_nameT id; vfastarr_pop(tokeniser->values, &id, sizeof(id));
      
      const size_t pos_top = vfastarr_elcnt(decl->data._declaration.expr, sizeof(*decl->data._declaration.expr));
      cth_astT* expr;
      if(id & 1){
        expr = cth_ast_init(CTH_TOK__NAME, tokeniser->line, tokeniser->offset, CTH_AST__ID);
        expr->data.id = id;
      }
      else
        expr = (cth_astT*)id;

      vfastarr_append(decl->data._declaration.expr, &expr, sizeof(expr));

      if(tok == CTH_TOK__COMMA){
        code += len;
        state = NAME;
        continue;
      }
      if(tok == CTH_TOK__SEMICOLON) break;
    }
  } while(1);

  tokeniser->values = vfastarr_append(tokeniser->values, &decl, sizeof(*tokeniser->values));
  tokeniser->state &= ~CTH_TOKENISER__DECLARATION;
  return code;
}


#endif
