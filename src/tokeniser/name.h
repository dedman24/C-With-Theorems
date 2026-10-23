#ifndef CTH_TOKENISER_NAME_H_INCLUDED
#define CTH_TOKENISER_NAME_H_INCLUDED

#include "types/chars.h"                  // char, string escape sequence handling.
#include "types/vtypes.h"                 // value types.
#include "../common/_common.h"            // error handling & more.

// stdlib includes.
#include "stddef.h"                       // size_t, NULL.
#include "stdint.h"                       // integer types.
#include "stdbool.h"                      // boolean types.
#include "inttypes.h"                     // sscanf formats.

typedef struct{
  cth_variable_ctxT* variables;   // NOTE: remember to extract this before deleting the whole name ctx.
  vfastbst_strT* names;
  vfastbst_strT values;
} cth_name_ctxT;

static void cth_name_ctx_init(cth_name_ctxT* const restrict name){
  name->variables = cth_variable_ctx_init(NULL);
  name->names = vfastarr_init(sizeof(*name->names));
  name->names = vfastarr_append(name->names, &VFASTBST_STR_INIT, sizeof(*name->names));
  name->names[0] = VFASTBST_STR_INIT;
  name->values = VFASTBST_STR_INIT;
}

// destroys name ctx, returns whether ctxdepth is 0 or not.
static bool cth_name_ctx_destroy(cth_name_ctxT* const restrict name, const bool freectx){
  const size_t ctxdepth = vfastarr_elcnt(name->names, sizeof(*name->names));
  for(size_t i = 0; i < ctxdepth; i++)
    vfastbst_str_destroy(&name->names[i], false);

  vfastarr_destroy(name->names);
  vfastbst_str_destroy(&name->values, false);
  if(freectx) free(name);

  return ctxdepth == 1;
}

static void cth_name_depth_inc(cth_name_ctxT* const restrict name){
  name->names = vfastarr_append(name->names, &VFASTBST_STR_INIT, sizeof(*name->names));
}

// returns false if depth goes below 0.
static bool cth_name_depth_dec(cth_name_ctxT* const restrict name){
  const size_t elcnt = vfastarr_elcnt(name, sizeof(*name));
  if(!elcnt) return false;

  vfastbst_str_destroy(&name->names[elcnt-1], false);
  vfastarr_pop(name->names, NULL, sizeof(*name->names));
  return true;
}

static size_t cth_name_depth(cth_name_ctxT* const restrict name){
  return vfastarr_elcnt(name, sizeof(*name));
}

// searches for name, creates nameID if it doesn't find any.
static cth_nameT cth_name_add(
  const char* const restrict key, const size_t s_key,
  cth_error_ctxT* const restrict err, cth_name_ctxT* const restrict name
){
  const size_t c_scopes = vfastarr_elcnt(name->names, sizeof(*name->names));

  vfastbst_str_nodeT** restrict curr_node = NULL;
  for(size_t i = 0; i < c_scopes; i++){
    curr_node = vfastbst_str_put_half(&name->names[i], key, s_key);
    if(*curr_node) 
      return (*curr_node)->obj;
  }

  const cth_nameT id = cth_variable_name(name->variables);
  *curr_node = vfastbst_str_node_init(key, s_key, id);
  cth_variable_add(name->variables, id, key, s_key, NULL, CTH_TYPE__NONE, CTH_QUALIFIER__NONE);
  return id;
}

// decodes binary string, returns 1 on success, 0 otherwise.
static int cth_value_decode__binary(
  const char* restrict data, const size_t s_data, uint64_t* const restrict raw, int* const restrict bytes_read
){
  *raw = 0;
  for(*bytes_read = 0; *bytes_read < s_data; (*bytes_read)++){
    if((unsigned char)data[*bytes_read] - '0' > 1) break;    
    *raw = (data[*bytes_read] - '0') << *bytes_read;
  }

  return *bytes_read != 0;
}

static const void* cth_value_decode(
  const char* restrict data, size_t s_data, size_t* const restrict s_raw, cth_nameT* const restrict type_raw,
  cth_error_ctxT* const restrict err, rcT* const restrict path, const cth_fposT line, const cth_fposT offset
){
  const cth_nameT type = cth_vtype(data, s_data, err, path, line, offset);
  if(type == CTH_TYPE__NONE) 
    return NULL;
  *type_raw = type;

  int bytes_read, args_read;
  char* restrict raw;
  const bool negated = data[0] == '-';
// I love C.
  enum{ HEX, DECIMAL, OCTAL, BINARY } encoding;
  if(
    s_data > 2 && data[0] == 0 &&
    type != CTH_TYPE__F8 && type != CTH_TYPE__F16 && type != CTH_TYPE__F32 && type != CTH_TYPE__F64
  ){
    if(negated){ data++; s_data--; }
    
    if(data[1] == 'x') encoding = HEX;
    else if(data[1] == 'b') encoding = BINARY;
    else if(data[1] == 'o' || (data[1] <= '0' && data[1] >= '9')) encoding = OCTAL;       // supports 0o, leading 0 octal formats.
    else{
      encoding = DECIMAL;
      if(negated){ data--; s_data++; }
    }
    
    if(encoding != DECIMAL){ data += 2; s_data -= 2; }
  }
  else encoding = DECIMAL;

  switch(type){
    case CTH_TYPE__F8:
    case CTH_TYPE__F16:
      cth_error_report(CTH_ERROR__VALUE_FORMAT_UNIMPLEMENTED, CTH_ERROR__COMPILER, path, line, offset, err);
      return NULL;
    case CTH_TYPE__F32:
      *s_raw = sizeof(float);
      raw = malloc(sizeof(float));
      args_read = sscanf(data, "%f%n", (float*)raw, &bytes_read);
      break;
    case CTH_TYPE__F64:
      *s_raw = sizeof(double);
      raw = malloc(sizeof(double));
      args_read = sscanf(data, "%lf%n", (double*)raw, &bytes_read);
      break;
    case CTH_TYPE__U8:
    case CTH_TYPE__U16:
    case CTH_TYPE__U32:
    case CTH_TYPE__U64:
      *s_raw = sizeof(uint64_t);
      raw = malloc(sizeof(uint64_t));
      switch(encoding){
        case HEX:
          args_read = sscanf(data, "%" SCNx64 "%n", (uint64_t*)raw, &bytes_read); 
          if(negated) *(uint64_t*)raw = -*(uint64_t*)raw;
          break;
        case DECIMAL:
          args_read = sscanf(data, "%" SCNu64 "%n", (uint64_t*)raw, &bytes_read);
          break;
        case OCTAL:
          args_read = sscanf(data, "%" SCNo64 "%n", (uint64_t*)raw, &bytes_read);
          if(negated) *(uint64_t*)raw = -*(uint64_t*)raw;
          break;
        case BINARY:
          args_read = cth_value_decode__binary(data, s_data, (void*)raw, &bytes_read);
          if(negated) *(uint64_t*)raw = -*(uint64_t*)raw;
          break;
      }
      break;
    case CTH_TYPE__I8:
    case CTH_TYPE__I16:
    case CTH_TYPE__I32:
    case CTH_TYPE__I64:
      *s_raw = sizeof(int64_t);
      raw = malloc(sizeof(int64_t));
      switch(encoding){
        case HEX:
          args_read = sscanf(data, "%" SCNx64 "%n", (int64_t*)raw, &bytes_read); 
          if(negated) *(int64_t*)raw = -*(int64_t*)raw;
          break;
        case DECIMAL:
          args_read = sscanf(data, "%" SCNd64 "%n", (int64_t*)raw, &bytes_read);
          break;
        case OCTAL:
          args_read = sscanf(data, "%" SCNo64 "%n", (int64_t*)raw, &bytes_read);
          if(negated) *(int64_t*)raw = -*(int64_t*)raw;
          break;
        case BINARY:
          args_read = cth_value_decode__binary(data, s_data, (void*)raw, &bytes_read);
          if(negated) *(int64_t*)raw = -*(int64_t*)raw;
          break;
      }
      break;
    case CTH_TYPE__NONE:      // error reported by cth_vtype.
      return NULL;
    default:                          // should not happen, very worrying if it does.
      cth_error_report__u32(CTH_ERROR__IMPOSSIBILITY, CTH_ERROR__COMPILER, path, line, offset, err, CTH_IMPOSSIBILITY__NAME_H_DEFAULT);
      return NULL;
  }

  if(bytes_read != s_data)
    cth_error_report__u32(CTH_ERROR__VALUE_TOO_FEW_BYTES, CTH_ERROR__COMPILER, path, line, offset, err, bytes_read);
  return raw;
}

// searches for value, creates nameID if it doesn't find any.
static cth_nameT cth_value_add(
  const char* const restrict data, const size_t s_data, cth_name_ctxT* const restrict name,
  cth_error_ctxT* const restrict err, rcT* const restrict path, const cth_fposT line, const cth_fposT offset
){
  size_t s_raw;
  cth_nameT type_raw;
  const char* const restrict raw = cth_value_decode(data, s_data, &s_raw, &type_raw, err, path, line, offset);

  vfastbst_str_nodeT** const restrict curr_node = vfastbst_str_put_half(&name->values, raw, s_raw);
  if(*curr_node){
    free((char*)raw);
    return (*curr_node)->obj;
  }

  const cth_nameT id = cth_variable_name(name->variables);
  *curr_node = vfastbst_str_node_init(raw, s_raw, id);
  cth_variable_add(name->variables, id, raw, s_raw, (void*)raw, type_raw, CTH_QUALIFIER__CONST);
  return id;
}

static cth_nameT cth_char_add(
  const char* const restrict data, const size_t s_data, cth_name_ctxT* const restrict name,
  cth_error_ctxT* const restrict err, rcT* const restrict path, const cth_fposT line, const cth_fposT offset
){
  size_t s_raw;
  char* const restrict raw = malloc(1);
  raw[0] = cth_vtype_char(data, s_data, true, &s_raw, err, path, line, offset);

  vfastbst_str_nodeT** const restrict curr_node = vfastbst_str_put_half(&name->values, raw, s_raw);
  if(*curr_node){
    free((char*)raw);
    return (*curr_node)->obj;
  }

  const cth_nameT id = cth_variable_name(name->variables);
  *curr_node = vfastbst_str_node_init(raw, s_raw, id);
  cth_variable_add(name->variables, id, raw, s_raw, (void*)raw, CTH_TYPE__CHAR, CTH_QUALIFIER__CONST);
  return id;
}

static cth_nameT cth_string_add(
  const char* const restrict data, const size_t s_data, cth_name_ctxT* const restrict name,
  cth_error_ctxT* const restrict err, rcT* const restrict path, const cth_fposT line, const cth_fposT offset
){
  size_t s_raw;
  const char* const restrict raw = cth_vtype_string(data, s_data, &s_raw, err, path, line, offset);

  vfastbst_str_nodeT** const restrict curr_node = vfastbst_str_put_half(&name->values, raw, s_raw);
  if(*curr_node){
    free((char*)raw);
    return (*curr_node)->obj;
  }

  const cth_nameT id = cth_variable_name(name->variables);
  *curr_node = vfastbst_str_node_init(raw, s_raw, id);
  cth_variable_add(name->variables, id, raw, s_raw, (void*)raw, CTH_TYPE__STRING, CTH_QUALIFIER__CONST);
  return id;
}

#endif
