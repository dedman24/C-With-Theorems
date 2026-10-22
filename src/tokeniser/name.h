#ifndef LTH_TOKENISER_NAME_H_INCLUDED
#define LTH_TOKENISER_NAME_H_INCLUDED

#include "types/chars.h"                  // char, string escape sequence handling.
#include "types/vtypes.h"                 // value types.
#include "../common/_common.h"            // error handling & more.

// stdlib includes.
#include "stddef.h"                       // size_t, NULL.
#include "stdint.h"                       // integer types.
#include "stdbool.h"                      // boolean types.
#include "inttypes.h"                     // sscanf formats.

typedef struct{
  lth_variable_ctxT* variables;   // NOTE: remember to extract this before deleting the whole name ctx.
  vfastbst_strT* names;
  vfastbst_strT values;
} lth_name_ctxT;

static void lth_name_ctx_init(lth_name_ctxT* const restrict name){
  name->variables = lth_variable_ctx_init(NULL);
  name->names = vfastarr_init(sizeof(*name->names));
  name->names = vfastarr_append(name->names, &VFASTBST_STR_INIT, sizeof(*name->names));
  name->names[0] = VFASTBST_STR_INIT;
  name->values = VFASTBST_STR_INIT;
}

// destroys name ctx, returns whether ctxdepth is 0 or not.
static bool lth_name_ctx_destroy(lth_name_ctxT* const restrict name, const bool freectx){
  const size_t ctxdepth = vfastarr_elcnt(name->names, sizeof(*name->names));
  for(size_t i = 0; i < ctxdepth; i++)
    vfastbst_str_destroy(&name->names[i], false);

  vfastarr_destroy(name->names);
  vfastbst_str_destroy(&name->values, false);
  if(freectx) free(name);

  return ctxdepth == 1;
}

static void lth_name_depth_inc(lth_name_ctxT* const restrict name){
  vfastarr_append(name->names, &VFASTBST_STR_INIT, sizeof(*name->names));
}

// returns false if depth goes below 0.
static bool lth_name_depth_dec(lth_name_ctxT* const restrict name){
  const size_t elcnt = vfastarr_elcnt(name, sizeof(*name));
  if(!elcnt) return false;

  vfastbst_str_destroy(&name->names[elcnt-1], false);
  vfastarr_pop(name->names, NULL, sizeof(*name->names));
  return true;
}

static size_t lth_name_depth(lth_name_ctxT* const restrict name){
  return vfastarr_elcnt(name, sizeof(*name));
}

// searches for name, creates nameID if it doesn't find any.
static lth_nameT lth_name_add(
  const char* const restrict key, const size_t s_key,
  lth_error_ctxT* const restrict err, lth_name_ctxT* const restrict name
){
  const size_t c_scopes = vfastarr_elcnt(name->names, sizeof(*name->names));

  vfastbst_str_nodeT** restrict curr_node = NULL;
  for(size_t i = 0; i < c_scopes; i++){
    curr_node = vfastbst_str_put_half(&name->names[i], key, s_key);
    if(*curr_node) 
      return (*curr_node)->obj;
  }

  const lth_nameT id = lth_variable_name(name->variables) | LTH_TOK__NAME_BIT;
  *curr_node = vfastbst_str_node_init(key, s_key, id);
  lth_variable_add(name->variables, id, key, s_key, NULL, LTH_TYPE__NONE, LTH_QUALIFIER__NONE);
  return id;
}

// decodes binary string, returns 1 on success, 0 otherwise.
static int lth_value_decode__binary(
  const char* restrict data, const size_t s_data, uint64_t* const restrict raw, int* const restrict bytes_read
){
  *raw = 0;
  for(*bytes_read = 0; *bytes_read < s_data; (*bytes_read)++){
    if((unsigned char)data[*bytes_read] - '0' > 1) break;    
    *raw = (data[*bytes_read] - '0') << *bytes_read;
  }

  return *bytes_read != 0;
}

static const void* lth_value_decode(
  const char* restrict data, size_t s_data, size_t* const restrict s_raw, lth_typeT* const restrict type_raw,
  lth_error_ctxT* const restrict err, rcT* const restrict path, const lth_fposT line, const lth_fposT offset
){
  const lth_typeT type = lth_vtype(data, s_data, err, path, line, offset);
  if(type == LTH_TYPE__NONE) 
    return NULL;
  *type_raw = type;

  int bytes_read, args_read;
  char* restrict raw;
  const bool negated = data[0] == '-';
// I love C.
  enum{ HEX, DECIMAL, OCTAL, BINARY } encoding;
  if(
    s_data > 2 && data[0] == 0 &&
    type != LTH_TYPE__F8 && type != LTH_TYPE__F16 && type != LTH_TYPE__F32 && type != LTH_TYPE__F64
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
    case LTH_TYPE__STRING:
      raw = lth_vtype_string(data + 1, s_data, s_raw, err, path, line, offset);
      return raw;
    case LTH_TYPE__CHAR:
      *s_raw = sizeof(char);
      raw = malloc(sizeof(char));
      *raw = lth_vtype_char(data + 1, s_data - 1, true, NULL, err, path, line, offset);
      return raw;
    case LTH_TYPE__F8:
    case LTH_TYPE__F16:
      lth_error_report(LTH_ERROR__VALUE_FORMAT_UNIMPLEMENTED, LTH_ERROR__COMPILER, path, line, offset, err);
      return NULL;
    case LTH_TYPE__F32:
      *s_raw = sizeof(float);
      raw = malloc(sizeof(float));
      args_read = sscanf(data, "%f%n", (float*)raw, &bytes_read);
      break;
    case LTH_TYPE__F64:
      *s_raw = sizeof(double);
      raw = malloc(sizeof(double));
      args_read = sscanf(data, "%lf%n", (double*)raw, &bytes_read);
      break;
    case LTH_TYPE__U8:
    case LTH_TYPE__U16:
    case LTH_TYPE__U32:
    case LTH_TYPE__U64:
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
          args_read = lth_value_decode__binary(data, s_data, (void*)raw, &bytes_read);
          if(negated) *(uint64_t*)raw = -*(uint64_t*)raw;
          break;
      }
      break;
    case LTH_TYPE__I8:
    case LTH_TYPE__I16:
    case LTH_TYPE__I32:
    case LTH_TYPE__I64:
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
          args_read = lth_value_decode__binary(data, s_data, (void*)raw, &bytes_read);
          if(negated) *(int64_t*)raw = -*(int64_t*)raw;
          break;
      }
      break;
    case LTH_TYPE__NONE:      // error reported by lth_vtype.
      return NULL;
    default:                          // should not happen, very worrying if it does.
      lth_error_report__u32(LTH_ERROR__IMPOSSIBILITY, LTH_ERROR__COMPILER, path, line, offset, err, LTH_IMPOSSIBILITY__NAME_H_DEFAULT);
      return NULL;
  }

  if(bytes_read != s_data)
    lth_error_report__u32(LTH_ERROR__VALUE_TOO_FEW_BYTES, LTH_ERROR__COMPILER, path, line, offset, err, bytes_read);
  return raw;
}

// searches for value, creates nameID if it doesn't find any.
static lth_nameT lth_value_add(
  const char* const restrict data, const size_t s_data, lth_name_ctxT* const restrict name,
  lth_error_ctxT* const restrict err, rcT* const restrict path, const lth_fposT line, const lth_fposT offset
){
  size_t s_raw;
  lth_typeT type_raw;
  const char* const restrict raw = lth_value_decode(data, s_data, &s_raw, &type_raw, err, path, line, offset);

  vfastbst_str_nodeT** const restrict curr_node = vfastbst_str_put_half(&name->values, raw, s_raw);
  if(*curr_node){
    free((char*)raw);
    return (*curr_node)->obj;
  }
// how are these handled? is 'rc' a proper way?
// 'rc' undoubtedly adds overhead, we don't really care about it being referenced multiple times.
  const lth_nameT id = lth_variable_name(name->variables);
  *curr_node = vfastbst_str_node_init(raw, s_raw, id);
  lth_variable_add(name->variables, id, raw, s_raw, (void*)raw, type_raw, LTH_QUALIFIER__CONST);
  return id | LTH_TOK__NAME_BIT;
}

#endif
