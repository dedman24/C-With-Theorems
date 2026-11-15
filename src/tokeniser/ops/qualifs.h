#ifndef CTH_TOKENISER_OPS_QUALIFS_H_INCLUDED
#define CTH_TOKENISER_OPS_QUALIFS_H_INCLUDED

#include "../ctx.h"
#include "../../interpreter/ops/typecheck.h"

struct cth_tokeniser_op__qualifierS{
  cth_nameT id;
  cth_variableT* var;
};

// combines type & qualifier to produce new qualified type name.
static struct cth_tokeniser_op__qualifierS cth_tokeniser_op__qualifier(
  cth_tokeniserT* const restrict tokeniser,
  const cth_nameT qual, cth_variableT* const restrict var_qual,
  const cth_nameT type, cth_variableT* const restrict var_type,
  const bool return_var,
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
  size_t s_qualif_name;
  char* qualif_name = cth_qualif_str_construct(NULL, var_qual, qual, tokeniser->name.variables, &s_qualif_name);

  size_t s_type_name = 1 + sizeof(cth_nameT) + var_type->s_name + 1 + s_qualif_name;
  char* const restrict type_name = malloc(s_type_name);
  type_name[0] = CTH_NAME_PFIX__TYPE;
  *(cth_nameT*)(type_name + 1) = type;
  memcpy(type_name + 1 + sizeof(cth_nameT), var_type->name, var_type->s_name);
  type_name[1 + sizeof(cth_nameT) + var_type->s_name] = ' ';
  memcpy(type_name + 1 + sizeof(cth_nameT) + var_type->s_name + 1, qualif_name, s_qualif_name);

  const cth_nameT seek = cth_name_search(type_name, s_type_name, &tokeniser->name);
  if(seek){
    cth_variableT* const var_seek = return_var? cth_variable_search(tokeniser->name.variables, seek): NULL;
    return (struct cth_tokeniser_op__qualifierS){ .id = seek, .var = var_seek };
  }

  const cth_nameT id_qualified = cth_variable_name(tokeniser->name.variables);
  cth_variableT* const restrict qualified = cth_variable_add(
    tokeniser->name.variables,
    id_qualified,
    type_name, s_type_name,
    var_type->type,
    var_type->state | CTH_VARSTATE__FREENAME
  );

  qualified->internal.type = cth_type__internal_clone(var_type->internal.type);
  cth_type__ast_init(qualified, qual);

  return (struct cth_tokeniser_op__qualifierS){ .id = id_qualified, .var = qualified };
}

static cth_astT* cth_tokeniser_op__make_compound_qualifier__funcall(const cth_nameT id_qualif, cth_astT* const restrict arg, const cth_fposT line, const cth_fposT offset){
  cth_astT* const restrict node = cth_ast_init(CTH_TOK__FUNCALL, line, offset, CTH_AST__FUNCALL);

  cth_astT* const restrict fun = cth_ast_init(CTH_TOK__NAME, line, offset, CTH_AST__ID);
  fun->data.id = id_qualif;

  node->data._funcall.fun = fun;
  node->data._funcall.args = vfastarr_init_explicit(1, 1, sizeof(*node->data._funcall.args));
  node->data._funcall.args[0] = arg;

  return node;
}

// constructs compound qualifier from two qualifiers, returns name.
// qualif1 must not be compound, qualif0 may be.
// constructs so that qualif0 is appended to qualif1.
static cth_nameT cth_tokeniser_op__make_compound_qualifier(
  cth_tokeniserT* const restrict tokeniser,
  const cth_nameT id_qualif0, cth_variableT* const restrict var_qualif0,
  const cth_nameT id_qualif1,
  rcT* const restrict path, cth_error_ctxT* const restrict err
){
  const cth_nameT id_compound = cth_variable_name(tokeniser->name.variables);
  cth_variableT* const compound = cth_variable_add(tokeniser->name.variables, id_compound, NULL, 0, cth_typevar(CTH_TYPE__QUALIFIER), CTH_VARSTATE__COMPOUNDQ);

  cth_astT* const restrict ast0 = var_qualif0->state & CTH_VARSTATE__COMPOUNDQ?
    cth_ast_clone(var_qualif0->internal.qualif):
    cth_tokeniser_op__make_compound_qualifier__funcall(id_qualif0, NULL, tokeniser->line, tokeniser->offset);

  cth_astT* const restrict ast1 = cth_tokeniser_op__make_compound_qualifier__funcall(id_qualif1, ast0, tokeniser->line, tokeniser->offset);
  compound->internal.qualif = ast1;

  return id_compound;
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
  if(!cth_typecheck_top_lvl_is(CTH_TYPE__TYPE, var_top->type))  // only valid on types.
    return false;

  const cth_nameT qualifier =
    tok == CTH_TOK__STAR?      CTH_QUALIFIER__PTR:
    tok == CTH_TOK__AMPERSAND? CTH_QUALIFIER__REF:
                               0;
// NOTE: possibly remove tokeniser seek, add similar mechanism to cth_typevar.
  cth_variableT* const restrict var_qualifier = cth_variable_search(tokeniser->name.variables, qualifier);

  const struct cth_tokeniser_op__qualifierS qualif = cth_tokeniser_op__qualifier(tokeniser, qualifier, var_qualifier, name_top, var_top, false, path, err);
  tokeniser->values[arr_top] = qualif.id;
  return true;
}

#endif
