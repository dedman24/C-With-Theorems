#ifndef CTH_TOKENISER_OPS_DECL_H_INCLUDED
#define CTH_TOKENISER_OPS_DECL_H_INCLUDED

#include "_includes.h"

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

  const cth_nameT id_master = cth_variable_name(tokeniser->variables);
  cth_variableT* var_master = cth_variable_add(tokeniser->variables, id_master, NULL, 0, cth_typevar(CTH_TYPE__TYPE), CTH_VARSTATE__NONE);
  var_master->internal.type = c_internal;
  return (struct cth_tokeniser_op__declaration_master_typeS){ .id = id_master, .var = var_master };
}

// handles declarations & their syntax.
static const char* cth_tokenise__declarations(
  cth_tokeniserT* const restrict tokeniser, const char* restrict code,
  rcT* const restrict path, cth_error_ctxT* const restrict err
){
  if(
    cth_tokeniser_scope_state_test(&tokeniser->scope, CTH_TOKENISER__DECLARATION) ||
    cth_tokeniser_scope_state_test(&tokeniser->scope, CTH_TOKENISER__TYPEDEF)
  ){
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

  const size_t s_values = cth_tokeniser_scope_values_sizeof(&tokeniser->scope),
               s_ops = cth_tokeniser_scope_ops_sizeof(&tokeniser->scope);
  if(s_values != sizeof(cth_nameT) || s_ops != 0){
    cth_error_report(CTH_ERROR__DECLARATION_WRONG, CTH_ERROR__COMPILER, path, tokeniser->line, tokeniser->offset, err);
    return code + strcspn(code, ";");
  }
// used to track current variable type.
  cth_nameT id_type = cth_tokeniser_scope_values_pull(&tokeniser->scope);
  cth_variableT* var_type = cth_variable_search(tokeniser->variables, id_type);
// used to track 'master' type.
  const struct cth_tokeniser_op__declaration_master_typeS master = cth_tokeniser_op__declaration_master_type(tokeniser, var_type);

  cth_astT* const restrict decl = cth_ast_init(CTH_TOK__DECLARATION, tokeniser->line, tokeniser->offset, CTH_AST__DECLARATION);
// declarations are modelled as a state machine.
// diagram:
//  NAME --CTH_TOK__NAME-> ASSIGN --CTH_TOK__ASSIGNMENT-> EXPR --CTH_TOK__SEMICOLON-> QUIT
//   ^------------------- CTH_TOK__COMMA -----------------+-|
  enum{ NAME, ASSIGN, EXPR } state = NAME;
// clears PREV_WAS_NAME bit, sets DECLARATION bit.
  cth_tokeniser_scope_state_andor(&tokeniser->scope, ~CTH_TOKENISER__PREV_WAS_NAME, CTH_TOKENISER__DECLARATION);
// parse everything until comma
  while(1){
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
      const struct cth_name_addS added = cth_name_add(&tokeniser->scope, tokeniser->variables, code, len, CTH_TYPE__NONE, CTH_VARSTATE__NONE);
      if(!added.var){
        if(!cth_qualifier_is__id(added.id)){
          cth_error_report__str_size(CTH_ERROR__DECLARATION_REDEFINITION, CTH_ERROR__COMPILER, path, tokeniser->line, tokeniser->offset, err, code, len);
          return code + strcspn(code, ";");
        }
        const struct cth_tokeniser_op__qualifierS qualif = cth_tokeniser_op__qualifier(tokeniser, added.id, added.var, id_type, var_type, true, path, err);
        id_type = qualif.id;
        var_type = qualif.var;
        continue;
      }

      added.var->type = var_type;
      decl->data._declaration.vars = vfastarr_append(decl->data._declaration.vars, &added.id, sizeof added.id);
      
      cth_astT* const ast_type = cth_ast_init(CTH_TOK__NAME, tokeniser->line, tokeniser->offset, CTH_AST__ID);
      ast_type->data.id = id_type;
      decl->data._declaration.types = vfastarr_append(decl->data._declaration.types, &ast_type, sizeof ast_type);
      var_type = master.var;
      id_type = master.id;

      code += len;
    }
    else if(state == ASSIGN){
// handles early exits due to smth like `i32 x;` or `i32 x, y;`.
      if(tok == CTH_TOK__COMMA || tok == CTH_TOK__SEMICOLON){
        cth_astT* expr = NULL;
        decl->data._declaration.expr = vfastarr_append(decl->data._declaration.expr, &expr, sizeof expr);
// 'code += len;' must not be executed on CTH_TOK__SEMICOLON due to how we handle it.
        if(tok == CTH_TOK__SEMICOLON)
          break;
        code += len;
        state = NAME;
        continue;
      }

      code += len;
      state = EXPR;

      if(tok != CTH_TOK__ASSIGNMENT){
        cth_error_report(CTH_ERROR__DECLARATION_EXPECTED_ID, CTH_ERROR__COMPILER, path, tokeniser->line, tokeniser->offset, err);
        code += strcspn(code, ";");
        break;
      }
    }
    else if(state == EXPR){
      if(tok != CTH_TOK__SEMICOLON && tok != CTH_TOK__COMMA){
        code = cth_tokeniser_dispatch(code, tok, len, tokeniser, path, err);
        continue;
      }
// NOTE: error is here!
      cth_tokeniser_dispatch(NULL, CTH_TOK__DECLARATION, 0, tokeniser, path, err);
      const cth_nameT id = cth_tokeniser_scope_values_pull(&tokeniser->scope);

      const size_t pos_top = vfastarr_elcnt(decl->data._declaration.expr, sizeof *decl->data._declaration.expr);
      cth_astT* expr;
      if(id & 1){
        expr = cth_ast_init(CTH_TOK__NAME, tokeniser->line, tokeniser->offset, CTH_AST__ID);
        expr->data.id = id;
      }
      else
        expr = (cth_astT*)id;

      decl->data._declaration.expr = vfastarr_append(decl->data._declaration.expr, &expr, sizeof expr);

      if(tok == CTH_TOK__COMMA){
        cth_tokeniser_scope_state_andor(&tokeniser->scope, ~CTH_TOKENISER__PREV_WAS_NAME, CTH_TOKENISER__DECLARATION);
        code += len;
        state = NAME;
        continue;
      }
      if(tok == CTH_TOK__SEMICOLON) break;
    }
  }

  cth_tokeniser_scope_values_push(&tokeniser->scope, (cth_nameT)decl);
  cth_tokeniser_scope_state_and(&tokeniser->scope, ~CTH_TOKENISER__DECLARATION);
  return code;
}

static const char* cth_tokenise__typedefs(
  cth_tokeniserT* const restrict tokeniser, const char* restrict code,
  rcT* const restrict path, cth_error_ctxT* const restrict err
){
  if(
    cth_tokeniser_scope_state_test(&tokeniser->scope, CTH_TOKENISER__DECLARATION) ||
    cth_tokeniser_scope_state_test(&tokeniser->scope, CTH_TOKENISER__TYPEDEF)
  ){
    cth_error_report(CTH_ERROR__DECLARATION_RECURSIVE, CTH_ERROR__COMPILER, path, tokeniser->line, tokeniser->offset, err);
    return code + strcspn(code, ";");
  }
  cth_tokeniser_scope_state_test(&tokeniser->scope, CTH_TOKENISER__TYPEDEF);
// typedefs have the following structure:
//  typedef type new-name;
// they can be handled in a similar manner as declarations, only using the NAME and ASSIGN state.
// constructs master type.  
  while(1){
    size_t len;
    const cth_tokT tok = cth_tok(code, &len);
    if(
      tok == CTH_TOK__OPENED_SINGLE_LINE_COMMENT ||
      tok == CTH_TOK__OPENED_MULTI_LINE_COMMENT ||
      tok == CTH_TOK__WHITESPACE ||
      tok == CTH_TOK__NEWLINE
    )
      code = cth_tokeniser_dispatch(code, tok, len, tokeniser, path, err);
    else if(tok == CTH_TOK__NAME){
      if(!cth_name_search(&tokeniser->scope, code, len))        // microopt possible here :DDD.
        break;
      code = cth_tokeniser_op__name(tokeniser, code, len, path, err);
    }
    else
      break;
  }

  cth_tokeniser_dispatch(NULL, CTH_TOK__DECLARATION, 0, tokeniser, path, err);
  const size_t s_values = cth_tokeniser_scope_values_sizeof(&tokeniser->scope), s_ops = cth_tokeniser_scope_ops_sizeof(&tokeniser->scope);
  if(s_values != sizeof(cth_nameT) || s_ops != 0){
    cth_error_report(CTH_ERROR__TYPEDEF_WRONG, CTH_ERROR__COMPILER, path, tokeniser->line, tokeniser->offset, err);
    return code + strcspn(code, ";");
  }
  cth_nameT id_master = cth_tokeniser_scope_values_pull(&tokeniser->scope);
  cth_variableT* const var_master = cth_variable_search(tokeniser->variables, id_master);

  cth_nameT id_type = id_master;
  cth_variableT* var_type = var_master;
// similar state machine as declarations.
// NAME --CTH_TOK__NAME-> COMMA --CTH_TOK__SEMICOLON-> QUIT
//  ^----------------------+-|
  enum{
    NAME,
    COMMA
  } state = NAME;

  cth_tokeniser_scope_state_or(&tokeniser->scope, CTH_TOKENISER__PREV_WAS_NAME);
  while(1){
    size_t len;
    cth_tokT tok = cth_tok(code, &len);
    
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
      cth_nameT exists;
// token-like qualifier, tries adding to variable.
      if(tok != CTH_TOK__NAME){
        const cth_nameT id_qualif = cth_tok__get_qualif_id(tok);
        if(!id_qualif){
          cth_error_report(CTH_ERROR__TYPEDEF_EXPECTED_ID, CTH_ERROR__COMPILER, path, tokeniser->line, tokeniser->offset, err);
          return code + strcspn(code, ";");
        }
        cth_variableT* const var_qualif = cth_variable_search(tokeniser->variables, id_qualif);
        const struct cth_tokeniser_op__qualifierS qualif = cth_tokeniser_op__qualifier(
          tokeniser,
          id_qualif, var_qualif,
          id_type, var_type,
          true, path, err
        );
        id_type = qualif.id;
        var_type = qualif.var;
      }
// name-like qualifier, tries adding to variable.
      else if((exists = cth_name_alias(&tokeniser->scope, code, len, id_type))){
        if(!cth_qualifier_is__id(exists)){
          cth_error_report__str_size(CTH_ERROR__TYPEDEF_REDEFINITION, CTH_ERROR__COMPILER, path, tokeniser->line, tokeniser->offset, err, code, len);
          return code + strcspn(code, ";");
        }
        cth_variableT* const var_exists = cth_variable_search(tokeniser->variables, exists);
        const struct cth_tokeniser_op__qualifierS qualif = cth_tokeniser_op__qualifier(tokeniser, exists, var_exists, id_type, var_type, true, path, err);
        id_type = qualif.id;
        var_type = qualif.var;
      }
// we add our entry as an alias of the type 'var_type' using cth_name_alias.
      else{
        var_type = var_master;
        id_type = id_master; 
        state = COMMA;
      }
      cth_tokeniser_scope_state_or(&tokeniser->scope, CTH_TOKENISER__PREV_WAS_NAME);
      code += len;
    }
    else{
      if(tok == CTH_TOK__SEMICOLON) break;
      state = NAME;
      var_type = var_master;
      id_type = id_master;

      code += len;
    }
  }

  cth_tokeniser_scope_state_and(&tokeniser->scope, ~(CTH_TOKENISER__PREV_WAS_NAME | CTH_TOKENISER__TYPEDEF));
  return code;
}

#endif
