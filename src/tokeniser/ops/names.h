#ifndef CTH_TOKENISER_OPS_NAMES_H_INCLUDED
#define CTH_TOKENISER_OPS_NAMES_H_INCLUDED

#include "decl.h"                 // declarations.
#include "../ctx.h"
#include "qualifs.h"

// stdlib includes.
#include "alloca.h"

static cth_nameT cth_tokeniser_op__name_join(
  cth_tokeniserT* const restrict tokeniser, const cth_nameT name, const cth_nameT type,
  rcT* const restrict path, cth_error_ctxT* const restrict err
){
  const cth_nameT combined = cth_type__combine(type, name);
  if(combined == CTH_TYPE__NONE)
    cth_error_report__type_2(CTH_ERROR__TYPE_UNJOINABLE, CTH_ERROR__COMPILER, path, tokeniser->line, tokeniser->offset, err, tokeniser->name.variables, type, name);
  return combined;
}

static const char* cth_tokeniser_op__name(
  cth_tokeniserT* const restrict tokeniser, const char* const restrict code, const size_t len,
  rcT* const restrict path, cth_error_ctxT* const restrict err
){
  if(!(tokeniser->state & CTH_TOKENISER__PREV_WAS_NAME)){
    tokeniser->state |= CTH_TOKENISER__PREV_WAS_NAME;
    const cth_nameT id = cth_name_search(code, len, &tokeniser->name);
    if(!id)
      cth_error_report(CTH_ERROR__VALUE_UNDEFINED, CTH_ERROR__COMPILER, path, tokeniser->line, tokeniser->offset, err);
    else
      tokeniser->values = vfastarr_append(tokeniser->values, &id, sizeof(id));
    return code+len;
  }

  cth_nameT id_prev; vfastarr_pop(tokeniser->values, &id_prev, sizeof(id_prev));
  cth_variableT* const var_prev = cth_variable_search(tokeniser->name.variables, id_prev);
  if(var_prev->type != CTH_TYPE__TYPE && var_prev->type != CTH_TYPE__QUALIFIER){
    cth_error_report(CTH_ERROR__TOKENISER_ADJACENT_OPERANDS, CTH_ERROR__COMPILER, path, tokeniser->line, tokeniser->offset, err);
    return code+len;
  }

  cth_name_addS spec = cth_name_add(code, len, id_prev, CTH_VARSTATE__NONE, &tokeniser->name);
// if spec.var == NULL, variable didn't exist previously. name is in spec.name.
  if(!spec.var)       // declaration.
    return cth_tokeniser_op__declaration(tokeniser, code, len, path, err);
  // prev & ours are both types; we try joining.
  if(var_prev->type == CTH_TYPE__TYPE && spec.var->type == CTH_TYPE__TYPE){
    const cth_nameT joined = cth_tokeniser_op__name_join(tokeniser, spec.name, id_prev, path, err);
    tokeniser->values = vfastarr_append(tokeniser->values, &joined, sizeof(joined));
    return code+len;
  }
  if(
    var_prev->type == CTH_TYPE__QUALIFIER && spec.var->type == CTH_TYPE__TYPE ||
    var_prev->type == CTH_TYPE__TYPE && spec.var->type == CTH_TYPE__QUALIFIER
  ){
    const bool first_isqualif = var_prev->type == CTH_TYPE__QUALIFIER;
    const cth_nameT name_qual = first_isqualif? id_prev: spec.name;
    const cth_nameT name_type = first_isqualif? spec.name: id_prev;
    cth_variableT* const tvar = first_isqualif? spec.var: var_prev;
    const cth_nameT qualified = cth_tokeniser_op__qualifier(tokeniser, name_qual, name_type, tvar, path, err);
    tokeniser->values = vfastarr_append(tokeniser->values, &qualified, sizeof(qualified));
    return code+len;
  }
  else{
    cth_error_report(CTH_ERROR__TOKENISER_ADJACENT_OPERANDS, CTH_ERROR__COMPILER, path, tokeniser->line, tokeniser->offset, err);
    return code+len;
  }
}

static void cth_tokeniser_op__value(
  cth_tokeniserT* const restrict tokeniser, const char* const restrict code, const size_t len,
  rcT* const restrict path, cth_error_ctxT* const restrict err
){
  if(tokeniser->state & CTH_TOKENISER__PREV_WAS_NAME)
    cth_error_report(CTH_ERROR__TOKENISER_ADJACENT_OPERANDS, CTH_ERROR__COMPILER, path, tokeniser->line, tokeniser->offset, err);
  else
    tokeniser->state |= CTH_TOKENISER__PREV_WAS_NAME;

  const cth_nameT value = cth_value_add(code, len, &tokeniser->name, err, path, tokeniser->line, tokeniser->offset);
  tokeniser->values = vfastarr_append(tokeniser->values, &value, sizeof(value));
}

static void cth_tokeniser_op__char(
  cth_tokeniserT* const restrict tokeniser, const char* const restrict code, const size_t len,
  rcT* const restrict path, cth_error_ctxT* const restrict err
){
  if(tokeniser->state & CTH_TOKENISER__PREV_WAS_NAME)
    cth_error_report(CTH_ERROR__TOKENISER_ADJACENT_OPERANDS, CTH_ERROR__COMPILER, path, tokeniser->line, tokeniser->offset, err);
  else
    tokeniser->state |= CTH_TOKENISER__PREV_WAS_NAME;

  const cth_nameT character = cth_char_add(code, len, &tokeniser->name, err, path, tokeniser->line, tokeniser->offset);
  tokeniser->values = vfastarr_append(tokeniser->values, &character, sizeof(character));
}

static void cth_tokeniser_op__string(
  cth_tokeniserT* const restrict tokeniser, const char* const restrict code, const size_t len,
  rcT* const restrict path, cth_error_ctxT* const restrict err
){
  if(tokeniser->state & CTH_TOKENISER__PREV_WAS_NAME)
    cth_error_report(CTH_ERROR__TOKENISER_ADJACENT_OPERANDS, CTH_ERROR__COMPILER, path, tokeniser->line, tokeniser->offset, err);
  else
    tokeniser->state |= CTH_TOKENISER__PREV_WAS_NAME;

  const cth_nameT string = cth_string_add(code, len, &tokeniser->name, err, path, tokeniser->line, tokeniser->offset);
  tokeniser->values = vfastarr_append(tokeniser->values, &string, sizeof(string));
}

static void cth_tokeniser_op__typedef(
  cth_tokeniserT* const restrict tokeniser,
  rcT* const restrict path, cth_error_ctxT* const restrict err
){
// typedefs are very ugly, screw them.
// I am not as productive as I'd like to be TwT...
  fprintf(stderr, "ERROR: typedefs not implemented yet.\n");
}

#endif
