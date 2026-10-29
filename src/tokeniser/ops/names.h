#ifndef CTH_TOKENISER_OPS_NAMES_H_INCLUDED
#define CTH_TOKENISER_OPS_NAMES_H_INCLUDED

#include "../ctx.h"

static const char* cth_tokeniser_op__name(
  cth_tokeniserT* const restrict tokeniser, const char* const restrict code, const size_t len,
  rcT* const restrict path, cth_error_ctxT* const restrict err
){
  cth_nameT type = CTH_TYPE__NONE;
  if(tokeniser->state & CTH_TOKENISER__PREV_WAS_NAME){
    if(!vfastarr_pop(tokeniser->values, &type, sizeof(type))){
      cth_error_report__u32(CTH_ERROR__IMPOSSIBILITY, CTH_ERROR__COMPILER, path, tokeniser->line, tokeniser->offset, err, CTH_IMPOSSIBILITY__TOKENISER_STATE_NAME);
      type = CTH_TYPE__TYPE;
    }
    cth_variableT* const restrict var = cth_variable_search(tokeniser->name.variables, type);
    if(!var){
      cth_error_report__u32(CTH_ERROR__IMPOSSIBILITY, CTH_ERROR__COMPILER, path, tokeniser->line, tokeniser->offset, err, CTH_IMPOSSIBILITY__VARIABLE_TYPE);
      type = CTH_TYPE__NONE;
    }
    else if(var->type != CTH_TYPE__TYPE){
      cth_error_report(CTH_ERROR__TOKENISER_ADJACENT_OPERANDS, CTH_ERROR__COMPILER, path, tokeniser->line, tokeniser->offset, err);
      type = CTH_TYPE__NONE;
    }
  }
  else
    tokeniser->state |= CTH_TOKENISER__PREV_WAS_NAME;

  const cth_nameT name = cth_name_add(code, len, type, CTH_QUALIFIER__NONE, err, &tokeniser->name);
  tokeniser->values = vfastarr_append(tokeniser->values, &name, sizeof(name));
  return code + len;
}

static const char* cth_tokeniser_op__value(
  cth_tokeniserT* const restrict tokeniser, const char* const restrict code, const size_t len,
  rcT* const restrict path, cth_error_ctxT* const restrict err
){
  if(tokeniser->state & CTH_TOKENISER__PREV_WAS_NAME)
    cth_error_report(CTH_ERROR__TOKENISER_ADJACENT_OPERANDS, CTH_ERROR__COMPILER, path, tokeniser->line, tokeniser->offset, err);
  else
    tokeniser->state |= CTH_TOKENISER__PREV_WAS_NAME;

  const cth_nameT value = cth_value_add(code, len, &tokeniser->name, err, path, tokeniser->line, tokeniser->offset);
  tokeniser->values = vfastarr_append(tokeniser->values, &value, sizeof(value));
  return code + len;
}

static const char* cth_tokeniser_op__char(
  cth_tokeniserT* const restrict tokeniser, const char* const restrict code, const size_t len,
  rcT* const restrict path, cth_error_ctxT* const restrict err
){
  if(tokeniser->state & CTH_TOKENISER__PREV_WAS_NAME)
    cth_error_report(CTH_ERROR__TOKENISER_ADJACENT_OPERANDS, CTH_ERROR__COMPILER, path, tokeniser->line, tokeniser->offset, err);
  else
    tokeniser->state |= CTH_TOKENISER__PREV_WAS_NAME;

  const cth_nameT character = cth_char_add(code, len, &tokeniser->name, err, path, tokeniser->line, tokeniser->offset);
  tokeniser->values = vfastarr_append(tokeniser->values, &character, sizeof(character));
  return code + len;
}

static const char* cth_tokeniser_op__string(
  cth_tokeniserT* const restrict tokeniser, const char* const restrict code, const size_t len,
  rcT* const restrict path, cth_error_ctxT* const restrict err
){
  if(tokeniser->state & CTH_TOKENISER__PREV_WAS_NAME)
    cth_error_report(CTH_ERROR__TOKENISER_ADJACENT_OPERANDS, CTH_ERROR__COMPILER, path, tokeniser->line, tokeniser->offset, err);
  else
    tokeniser->state |= CTH_TOKENISER__PREV_WAS_NAME;

  const cth_nameT string = cth_string_add(code, len, &tokeniser->name, err, path, tokeniser->line, tokeniser->offset);
  tokeniser->values = vfastarr_append(tokeniser->values, &string, sizeof(string));
  return code + len;
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
