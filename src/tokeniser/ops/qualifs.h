#ifndef CTH_TOKENISER_OPS_QUALIFS_H_INCLUDED
#define CTH_TOKENISER_OPS_QUALIFS_H_INCLUDED

#include "../ctx.h"

// combines type & qualifier to produce new qualified type name.
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
// throughout this we possibly update `tokeniser->d_state.type` accordingly.

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
  if(seek)
    return seek;

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

// tries to qualify using those qualifiers that specify a different type.
static bool cth_tokeniser__operand_qualifs(
  const cth_tokT tok, cth_tokeniserT* const restrict tokeniser,
  rcT* const restrict path, cth_error_ctxT* const restrict err
){
  if(tok != CTH_TOK__STAR && tok != CTH_TOK__AMPERSAND)
    return false;

  const size_t elcnt = vfastarr_elcnt(tokeniser->values, sizeof(*tokeniser->values));
  if(!elcnt)
    return false;
  const size_t arr_top = elcnt-1;

  const cth_nameT name_top = tokeniser->values[arr_top];
  cth_variableT* const restrict var_top = cth_variable_search(tokeniser->name.variables, name_top);
  if(var_top->type != CTH_TYPE__TYPE)     // only valid on types.
    return false;

  const cth_nameT qualifier = 
    tok == CTH_TOK__STAR?      CTH_QUALIFIER__PTR:
    tok == CTH_TOK__AMPERSAND? CTH_QUALIFIER__REF:
                               0;

  tokeniser->values[arr_top] = cth_tokeniser_op__qualifier(tokeniser, qualifier, name_top, var_top, path, err);
  return true;
}

#endif
