#ifndef CTH_TOKENISER_SCOPE_H_INCLUDED
#define CTH_TOKENISER_SCOPE_H_INCLUDED


#include "../../common/_common.h"
#include "../../interpreter/ast.h"

typedef struct{
  cth_astT* program;
  cth_tokT* ops;
  cth_nameT* values;
  vfastbst_strT names;
} cth_tokeniser_scope__entryT;

typedef struct{
  vfastextT internal;
} cth_tokeniser_scopeT;

static void cth_tokeniser_scope_name_put(
  cth_tokeniser_scopeT* const restrict scope,
  const char* const restrict key, const size_t s_key, const cth_nameT id
);

// initialises scope data.
static void cth_tokeniser_scope_init(cth_tokeniser_scopeT* const restrict scope){
// program scope.
// '8' chosen as optimistic; rarely do programs indent depeer than like 6, so smth like 8 is plenty conservative.
  cth_tokeniser_scope__entryT entry = {
    .program = vfastarr_init_explicit(1, 8, sizeof *entry.program),
    .ops     = vfastarr_init_explicit(1, 8, sizeof *entry.ops),
    .values  = vfastarr_init_explicit(1, 8, sizeof *entry.values),
    .names   = VFASTBST_STR_INIT,
  };

  vfastbst_str_put(&entry.names, cth_variable_name__NULL,  strlen(cth_variable_name__NULL),  CTH_NAME__NULL);
  vfastbst_str_put(&entry.names, cth_variable_name__true,  strlen(cth_variable_name__true),  CTH_NAME__TRUE);
  vfastbst_str_put(&entry.names, cth_variable_name__false, strlen(cth_variable_name__false), CTH_NAME__FALSE);

  for(size_t i = CTH_TYPE_MIN; i < CTH_TYPE_MAX; i += 2)
    vfastbst_str_put(&entry.names, cth_type__strings[i], strlen(cth_type__strings[i]), i);
  for(size_t i = CTH_QUALIFIER_MIN; i < CTH_QUALIFIER_MAX; i += 2)
    vfastbst_str_put(&entry.names, cth_qualifier__strings[i], strlen(cth_qualifier__strings[i]), i);

  vfastext_append(&scope->internal, &entry, sizeof entry);
}

// destroys scope, returns last element of program of last scope.
static cth_astT* cth_tokeniser_scope_destroy(cth_tokeniser_scopeT* const restrict scope){
  cth_astT* prog0; vfastarr_top(((cth_tokeniser_scope__entryT*)scope->internal.arr)[0].program, &prog0, sizeof prog0);
  const size_t c_scopes = vfastext_countof(&scope->internal, sizeof(cth_tokeniser_scope__entryT));

  for(size_t i = 0; i < c_scopes; i++){
    cth_tokeniser_scope__entryT* const entry = &scope->internal.arr[i];
    vfastarr_destroy(entry->ops);
    vfastarr_destroy(entry->values);

    cth_ast_destroy(entry->program);
    vfastbst_str_destroy(&entry->names);
  }

  return prog0;
}

static void cth_tokeniser_scope_inc(cth_tokeniser_scopeT* const restrict scope){
  cth_tokeniser_scope__entryT entry = {
    .program = vfastarr_init_explicit(1, 8, sizeof *entry.program),
    .ops     = vfastarr_init_explicit(1, 8, sizeof *entry.ops),
    .values  = vfastarr_init_explicit(1, 8, sizeof *entry.values),
    .names   = VFASTBST_STR_INIT,
  };

  vfastext_append(&scope->internal, &entry, sizeof entry);
}

static bool cth_tokeniser_scope_dec(cth_tokeniser_scopeT* const restrict scope){
  const size_t c_scopes = vfastext_countof(&scope->internal, sizeof(cth_tokeniser_scope__entryT));
  if(c_scopes <= 1) return false;

  cth_tokeniser_scope__entryT* const e_scopes = scope->internal.arr;
// cleans up main stuff.
  vfastarr_destroy(e_scopes[c_scopes - 1].ops);
  vfastarr_destroy(e_scopes[c_scopes - 1].values);
  vfastbst_str_destroy(&e_scopes[c_scopes - 1].names);
// cleans up program fiel, which has special handling.
  cth_astT* const appended = e_scopes[c_scopes - 1].program;
// shorthand names.
  cth_astT** a_ast = e_scopes[c_scopes - 2].program->data.program.program; 
  cth_tokT* a_ops = e_scopes[c_scopes - 2].ops;
  cth_nameT* a_val = e_scopes[c_scopes - 2].values;
  if(!vfastarr_size(a_ops) && !vfastarr_size(a_val))
    a_ast = vfastarr_append(a_ast, &appended, sizeof *a_ast);
  else
    a_val = vfastarr_append(a_val, &appended, sizeof *a_val);

  vfastext_remove(&scope->internal, sizeof(cth_tokeniser_scope__entryT));
  return true;
}


static size_t cth_tokeniser_scope_ops_sizeof(cth_tokeniser_scopeT* const restrict scope){
  const size_t c_scopes = vfastext_countof(&scope->internal, sizeof(cth_tokeniser_scope__entryT));
  return vfastarr_size(((cth_tokeniser_scope__entryT*)scope->internal.arr)[c_scopes-1].ops);
}

static size_t cth_tokeniser_scope_ops_countof(cth_tokeniser_scopeT* const restrict scope){
  const size_t c_scopes = vfastext_countof(&scope->internal, sizeof(cth_tokeniser_scope__entryT));
  return vfastarr_elcnt(
    ((cth_tokeniser_scope__entryT*)scope->internal.arr)[c_scopes-1].ops,
    sizeof(cth_tokT)
  );
}

static void cth_tokeniser_scope_ops_push(cth_tokeniser_scopeT* const restrict scope, cth_tokT op){
  const size_t c_scopes = vfastext_countof(&scope->internal, sizeof(cth_tokeniser_scope__entryT));
  ((cth_tokeniser_scope__entryT*)scope->internal.arr)[c_scopes-1].ops = vfastarr_append(
    ((cth_tokeniser_scope__entryT*)scope->internal.arr)[c_scopes-1].ops,
    &op, sizeof op
  );
}

static cth_tokT cth_tokeniser_scope_ops_pull(cth_tokeniser_scopeT* const restrict scope){
  const size_t c_scopes = vfastext_countof(&scope->internal, sizeof(cth_tokeniser_scope__entryT));
  cth_tokT op;
  vfastarr_pop(
    ((cth_tokeniser_scope__entryT*)scope->internal.arr)[c_scopes-1].ops,
    &op, sizeof op
  );
  return op;
}


static size_t cth_tokeniser_scope_values_sizeof(cth_tokeniser_scopeT* const restrict scope){
  const size_t c_scopes = vfastext_countof(&scope->internal, sizeof(cth_tokeniser_scope__entryT));
  return vfastarr_size(((cth_tokeniser_scope__entryT*)scope->internal.arr)[c_scopes-1].values);
}

static size_t cth_tokeniser_scope_values_countof(cth_tokeniser_scopeT* const restrict scope){
  const size_t c_scopes = vfastext_countof(&scope->internal, sizeof(cth_tokeniser_scope__entryT));
  return vfastarr_elcnt(
    ((cth_tokeniser_scope__entryT*)scope->internal.arr)[c_scopes-1].values,
    sizeof(cth_nameT)
  );
}

static void cth_tokeniser_scope_values_push(cth_tokeniser_scopeT* const restrict scope, cth_nameT value){
  const size_t c_scopes = vfastext_countof(&scope->internal, sizeof(cth_tokeniser_scope__entryT));
  ((cth_tokeniser_scope__entryT*)scope->internal.arr)[c_scopes-1].values = vfastarr_append(
    ((cth_tokeniser_scope__entryT*)scope->internal.arr)[c_scopes-1].values,
    &value, sizeof value
  );
}

static cth_nameT cth_tokeniser_scope_values_pull(cth_tokeniser_scopeT* const restrict scope){
  const size_t c_scopes = vfastext_countof(&scope->internal, sizeof(cth_tokeniser_scope__entryT));
  cth_nameT value;
  vfastarr_pop(
    ((cth_tokeniser_scope__entryT*)scope->internal.arr)[c_scopes-1].values,
    &value, sizeof value
  );
  return value;
}


static size_t cth_tokeniser_scope_program_sizeof(cth_tokeniser_scopeT* const restrict scope){
  const size_t c_scopes = vfastext_countof(&scope->internal, sizeof(cth_tokeniser_scope__entryT));
  return vfastarr_size(((cth_tokeniser_scope__entryT*)scope->internal.arr)[c_scopes-1].program->data.program.program);
}

static size_t cth_tokeniser_scope_program_countof(cth_tokeniser_scopeT* const restrict scope){
  const size_t c_scopes = vfastext_countof(&scope->internal, sizeof(cth_tokeniser_scope__entryT));
  return vfastarr_elcnt(
    ((cth_tokeniser_scope__entryT*)scope->internal.arr)[c_scopes-1].program->data.program.program,
    sizeof(cth_tokT)
  );
}

static void cth_tokeniser_scope_program_push(cth_tokeniser_scopeT* const restrict scope, cth_astT* node){
  const size_t c_scopes = vfastext_countof(&scope->internal, sizeof(cth_tokeniser_scope__entryT));
  ((cth_tokeniser_scope__entryT*)scope->internal.arr)[c_scopes-1].program->data.program.program = vfastarr_append(
    ((cth_tokeniser_scope__entryT*)scope->internal.arr)[c_scopes-1].program->data.program.program,
    &node, sizeof node
  );
}

static cth_astT* cth_tokeniser_scope_program_pull(cth_tokeniser_scopeT* const restrict scope){
  const size_t c_scopes = vfastext_countof(&scope->internal, sizeof(cth_tokeniser_scope__entryT));
  cth_astT* node;
  vfastarr_pop(
    ((cth_tokeniser_scope__entryT*)scope->internal.arr)[c_scopes-1].program->data.program.program,
    &node, sizeof node
  );
  return node;
}

static void cth_tokeniser_scope_name_put(
  cth_tokeniser_scopeT* const restrict scope,
  const char* const restrict key, const size_t s_key, const cth_nameT id
){
  const size_t c_scopes = vfastext_countof(&scope->internal, sizeof(cth_tokeniser_scope__entryT));
  vfastbst_strT* const names = &((cth_tokeniser_scope__entryT*)scope->internal.arr + c_scopes)->names;
  vfastbst_str_put(names, key, s_key, id);
}

struct cth_name_addS{
  cth_nameT id;
  cth_variableT* var;
};

// searches for name, creates nameT if it doesn't find any.
// returns .var field set to NULL if it already exists.
static struct cth_name_addS cth_name_add(
  cth_tokeniser_scopeT* const restrict scope, cth_variable_ctxT* const restrict variables,
  const char* const restrict key, const size_t s_key,
  const cth_nameT type, const cth_variable_stateT state
){
  const size_t c_scopes = vfastext_countof(&scope->internal, sizeof(cth_tokeniser_scope__entryT));
  cth_tokeniser_scope__entryT* const e_scopes = scope->internal.arr;

  vfastbst_str_nodeT** restrict curr_node = NULL;
  for(size_t i = c_scopes - 1; i != -1; i--){
    curr_node = vfastbst_str_put_half(&e_scopes[i].names, key, s_key);
    if(*curr_node)
      return (struct cth_name_addS){ .id = (*curr_node)->obj, .var = NULL };
  }

// 'signed', 'unsigned' are not actual valid types values can assume, they're aliases of 'int' & 'unsigned int' when it's relevant.
  cth_variableT* const typevar = 
    type == CTH_TYPE__SIGNED?   cth_typevar(CTH_TYPE__INT):
    type == CTH_TYPE__UNSIGNED? cth_typevar(CTH_TYPE__UINT):
    cth_variable_search(variables, type);

  const cth_nameT id = cth_variable_name(variables);
  *curr_node = vfastbst_str_node_init(key, s_key, id);
  cth_variableT* const var = cth_variable_add(variables, id, key, s_key, typevar, state);
  return (struct cth_name_addS){ .id = id, .var = var };
}

// adds name alias to given variable.
// returns 0 if successful, proper name if it isn't.
static cth_nameT cth_name_alias(
  cth_tokeniser_scopeT* const restrict scope,
  const char* const restrict key, const size_t s_key, const cth_nameT aliased
){
  const size_t c_scopes = vfastext_countof(&scope->internal, sizeof(cth_tokeniser_scope__entryT));
  cth_tokeniser_scope__entryT* const e_scopes = scope->internal.arr;

  vfastbst_str_nodeT** restrict curr_node = NULL;
  for(size_t i = c_scopes - 1; i != -1; i--){
    curr_node = vfastbst_str_put_half(&e_scopes[i].names, key, s_key);
    if(*curr_node)
      return (*curr_node)->obj;
  }
  *curr_node = vfastbst_str_node_init(key, s_key, aliased);
  return 0;
}

// searches for name by string, returns 0 on error.
static cth_nameT cth_name_search(
  cth_tokeniser_scopeT* const restrict scope,
  const char* const restrict key, const size_t s_key
){
  const size_t c_scopes = vfastext_countof(&scope->internal, sizeof(cth_tokeniser_scope__entryT));
  cth_tokeniser_scope__entryT* const e_scopes = scope->internal.arr;

  vfastbst_str_nodeT** restrict curr_node = NULL;
  for(size_t i = 0; i < c_scopes; i++){
    const cth_nameT seek = vfastbst_str_search(&e_scopes[i].names, key, s_key);
    if(seek)
      return seek;
  }

  return 0;
}

#endif
