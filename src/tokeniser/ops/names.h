#ifndef CTH_TOKENISER_OPS_NAMES_H_INCLUDED
#define CTH_TOKENISER_OPS_NAMES_H_INCLUDED

#include "_includes.h"

#include "decl.h"                               // declarations.
#include "qualifs.h"
#include "../../interpreter/ops/typecheck.h"    // typechecking & type-related ops.

// stdlib includes.
#include "alloca.h"

static cth_nameT cth_tokeniser_op__name_join(
  cth_tokeniserT* const restrict tokeniser, const cth_nameT name, const cth_nameT type,
  rcT* const restrict path, cth_error_ctxT* const restrict err
){
  const cth_nameT combined = cth_type__combine(type, name);
  if(combined == CTH_TYPE__NONE)
    cth_error_report__type_2(
      CTH_ERROR__TYPE_UNJOINABLE, CTH_ERROR__COMPILER, 
      path, tokeniser->line, tokeniser->offset, err,
      tokeniser->variables,
      cth_variable_search(tokeniser->variables, type),
      cth_variable_search(tokeniser->variables, name)
    );
  return combined;
}

// save for declarations, always returns code + len.
static const char* cth_tokeniser_op__name(
  cth_tokeniserT* const restrict tokeniser, const char* const restrict code, const size_t len,
  rcT* const restrict path, cth_error_ctxT* const restrict err
){
  if(!cth_tokeniser_scope_state_test(&tokeniser->scope, CTH_TOKENISER__PREV_WAS_NAME)){
    cth_tokeniser_scope_state_andor(&tokeniser->scope, ~CTH_TOKENISER__UNARY_ALLOWED, CTH_TOKENISER__PREV_WAS_NAME);
    const cth_nameT id = cth_name_search(&tokeniser->scope, code, len);
    if(!id)
      cth_error_report(CTH_ERROR__VALUE_UNDEFINED, CTH_ERROR__COMPILER, path, tokeniser->line, tokeniser->offset, err);
    else
      cth_tokeniser_scope_values_push(&tokeniser->scope, id);
    return code + len;
  }

  const cth_nameT name = cth_name_search(&tokeniser->scope, code, len);
// if spec.var == NULL, variable didn't exist previously. name is in spec.name.
  if(!name)
    return cth_tokenise__declarations(tokeniser, code, path, err);
  cth_variableT* const var = cth_variable_search(tokeniser->variables, name);

  const cth_nameT id_prev = cth_tokeniser_scope_values_pull(&tokeniser->scope);
  cth_variableT* const var_prev = cth_variable_search(tokeniser->variables, id_prev);
  if(!cth_typecheck_top_lvl_is(CTH_TYPE__TYPE, var_prev->type) && !cth_typecheck_top_lvl_is(CTH_TYPE__QUALIFIER, var_prev->type)){
    cth_error_report(CTH_ERROR__TOKENISER_ADJACENT_OPERANDS, CTH_ERROR__COMPILER, path, tokeniser->line, tokeniser->offset, err);
    return code + len;
  }
// prev & ours are both types; we try joining.
  if(cth_typecheck_top_lvl_is(CTH_TYPE__TYPE, var_prev->type) && cth_typecheck_top_lvl_is(CTH_TYPE__TYPE, var->type)){
    const cth_nameT joined = cth_tokeniser_op__name_join(tokeniser, name, id_prev, path, err);
    if(joined != CTH_TYPE__NONE)
      cth_tokeniser_scope_values_push(&tokeniser->scope, joined);
    return code + len;
  }
  if(
    cth_typecheck_top_lvl_is(CTH_TYPE__QUALIFIER, var_prev->type) && cth_typecheck_top_lvl_is(CTH_TYPE__TYPE, var->type) ||
    cth_typecheck_top_lvl_is(CTH_TYPE__TYPE, var_prev->type) && cth_typecheck_top_lvl_is(CTH_TYPE__QUALIFIER, var->type)
  ){
    const bool first_isqualif = cth_typecheck_top_lvl_is(CTH_TYPE__QUALIFIER, var_prev->type);
    const cth_nameT name_qual = first_isqualif? id_prev: name;
    const cth_nameT name_type = first_isqualif? name: id_prev;
    cth_variableT* const var_qual = first_isqualif? var_prev: var;
    cth_variableT* const var_type = first_isqualif? var: var_prev;
    const struct cth_tokeniser_op__qualifierS result = cth_tokeniser_op__qualifier(
      tokeniser,
      name_qual, var_qual,
      name_type, var_type,
      false, path, err
    );
    cth_tokeniser_scope_values_push(&tokeniser->scope, result.id);
    return code + len;
  }
  if(
    cth_typecheck_top_lvl_is(CTH_TYPE__QUALIFIER, var_prev->type) &&
    cth_typecheck_top_lvl_is(CTH_TYPE__QUALIFIER, var->type)
  ){
    const cth_nameT compound = cth_tokeniser_op__make_compound_qualifier(
      tokeniser,
      id_prev, var_prev,
      name,
      path, err
    );
    cth_tokeniser_scope_values_push(&tokeniser->scope, compound);
    return code + len;
  }
  if(cth_tokeniser_scope_state_test(&tokeniser->scope, CTH_TOKENISER__DECLARATION))     // redefinition error handled somewhere else.
    cth_error_report(CTH_ERROR__TOKENISER_ADJACENT_OPERANDS, CTH_ERROR__COMPILER, path, tokeniser->line, tokeniser->offset, err);
  return code + len;
}

static void cth_tokeniser_op__value(
  cth_tokeniserT* const restrict tokeniser, const char* const restrict code, const size_t len,
  rcT* const restrict path, cth_error_ctxT* const restrict err
){
  if(cth_tokeniser_scope_state_test(&tokeniser->scope, CTH_TOKENISER__PREV_WAS_NAME))
    cth_error_report(CTH_ERROR__TOKENISER_ADJACENT_OPERANDS, CTH_ERROR__COMPILER, path, tokeniser->line, tokeniser->offset, err);
  else
    cth_tokeniser_scope_state_andor(&tokeniser->scope, ~CTH_TOKENISER__UNARY_ALLOWED, CTH_TOKENISER__PREV_WAS_NAME);

  const cth_nameT value = cth_value_add(code, len, tokeniser, err, path, tokeniser->line, tokeniser->offset);
  cth_tokeniser_scope_values_push(&tokeniser->scope, value);
}

static void cth_tokeniser_op__char(
  cth_tokeniserT* const restrict tokeniser, const char* const restrict code, const size_t len,
  rcT* const restrict path, cth_error_ctxT* const restrict err
){
  if(cth_tokeniser_scope_state_test(&tokeniser->scope, CTH_TOKENISER__PREV_WAS_NAME))
    cth_error_report(CTH_ERROR__TOKENISER_ADJACENT_OPERANDS, CTH_ERROR__COMPILER, path, tokeniser->line, tokeniser->offset, err);
  else
    cth_tokeniser_scope_state_andor(&tokeniser->scope, ~CTH_TOKENISER__UNARY_ALLOWED, CTH_TOKENISER__PREV_WAS_NAME);

  const cth_nameT character = cth_char_add(code, len, tokeniser, err, path, tokeniser->line, tokeniser->offset);
  cth_tokeniser_scope_values_push(&tokeniser->scope, character);
}

static void cth_tokeniser_op__string(
  cth_tokeniserT* const restrict tokeniser, const char* const restrict code, const size_t len,
  rcT* const restrict path, cth_error_ctxT* const restrict err
){
  if(cth_tokeniser_scope_state_test(&tokeniser->scope, CTH_TOKENISER__PREV_WAS_NAME))
    cth_error_report(CTH_ERROR__TOKENISER_ADJACENT_OPERANDS, CTH_ERROR__COMPILER, path, tokeniser->line, tokeniser->offset, err);
  else
    cth_tokeniser_scope_state_andor(&tokeniser->scope, ~CTH_TOKENISER__UNARY_ALLOWED, CTH_TOKENISER__PREV_WAS_NAME);

  const cth_nameT string = cth_string_add(code, len, tokeniser, err, path, tokeniser->line, tokeniser->offset);
  cth_tokeniser_scope_values_push(&tokeniser->scope, string);
}

#endif
