#ifndef CTH_TOKENISER_OPS_NAMES_H_INCLUDED
#define CTH_TOKENISER_OPS_NAMES_H_INCLUDED

#include "../ctx.h"

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

static cth_nameT cth_tokeniser_op__qualifier(
  cth_tokeniserT* const restrict tokeniser, const cth_nameT qualifier, const cth_nameT type, cth_variableT* const restrict type_var,
  rcT* const restrict path, cth_error_ctxT* const restrict err
){
// algorithm:
//  construct type name.
//  search for type ~ return if it exists.
//  clone type variable.
//  modify tree by adding fcall to our qualifier.
//  return new type name.

// structure of type name:
// CTH_NAME_PFIX__TYPE underlying_type_nameT " " "space-separated qualifiers".
  const size_t s_qualifier = strlen(cth_qualifier__strings[qualifier]);

  size_t s_type_name = 1 + sizeof(cth_nameT) + type_var->s_name + 1 + s_qualifier;
  char* const restrict type_name = malloc(s_type_name);
  type_name[0] = CTH_NAME_PFIX__TYPE;
  *(cth_nameT*)(type_name + 1) = type;
  memcpy(type_name + 1 + sizeof(cth_nameT), type_var->name, type_var->s_name);
  type_name[1 + sizeof(cth_nameT) + type_var->s_name] = ' ';
  memcpy(type_name + 1 + sizeof(cth_nameT) + type_var->s_name + 1, cth_qualifier__strings[qualifier], s_qualifier);

  const cth_nameT seek = cth_name_search(type_name, s_type_name, &tokeniser->name);
  if(seek) return seek;

  const cth_nameT id_qualified = cth_variable_name(tokeniser->name.variables);
  cth_variableT* const restrict qualified = cth_variable_add(
    tokeniser->name.variables,
    id_qualified,
    type_name, s_type_name,
    type_var->type,
    type_var->state
  );

  qualified->internal.type = cth_type__internal_clone(type_var->internal.type);
  cth_type__ast_init(qualified, qualifier);

  return id_qualified;
}

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
  }
  else
    tokeniser->state |= CTH_TOKENISER__PREV_WAS_NAME;

  cth_name_addS spec = cth_name_add(code, len, type, CTH_VARSTATE__NONE, &tokeniser->name);
// if spec.var == NULL, variable doesn't exist. name is in spec.name.
  if(!spec.var){
    if(!spec.name)
      cth_error_report(CTH_ERROR__TOKENISER_ADJACENT_OPERANDS, CTH_ERROR__COMPILER, path, tokeniser->line, tokeniser->offset, err);
    else
      tokeniser->values = vfastarr_append(tokeniser->values, &spec.name, sizeof(spec.name));
    return code + len;
  }
// C variable joining & qualifier handling.
  if(type == CTH_TYPE__NONE){
    tokeniser->values = vfastarr_append(tokeniser->values, &spec.name, sizeof(spec.name));
    return code + len;
  }

  cth_variableT* const restrict type_var = cth_variable_search(tokeniser->name.variables, type);
  if(!type_var){
    cth_error_report__u32(CTH_ERROR__IMPOSSIBILITY, CTH_ERROR__COMPILER, path, tokeniser->line, tokeniser->offset, err, CTH_IMPOSSIBILITY__VARIABLE_TYPE);
    return 0;
  }

  if(type_var->type == CTH_TYPE__TYPE && spec.var->type == CTH_TYPE__TYPE)
    spec.name = cth_tokeniser_op__name_join(tokeniser, spec.name, type, path, err);
  else if(type_var->type == CTH_TYPE__TYPE && spec.var->type == CTH_TYPE__QUALIFIER)
    spec.name = cth_tokeniser_op__qualifier(tokeniser, spec.name, type, type_var, path, err);
  else if(type_var->type == CTH_TYPE__QUALIFIER && spec.var->type == CTH_TYPE__TYPE)
    spec.name = cth_tokeniser_op__qualifier(tokeniser, type, spec.name, spec.var, path, err);
  else if(type_var->type != CTH_TYPE__TYPE)
    cth_error_report(CTH_ERROR__TOKENISER_ADJACENT_OPERANDS, CTH_ERROR__COMPILER, path, tokeniser->line, tokeniser->offset, err);

  tokeniser->values = vfastarr_append(tokeniser->values, &spec.name, sizeof(spec.name));
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
