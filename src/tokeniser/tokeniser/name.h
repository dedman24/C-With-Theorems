#ifndef CTH_TOKENISER_NAME_H_INCLUDED
#define CTH_TOKENISER_NAME_H_INCLUDED

#include "ctx_full.h"                     // tokeniser ctx.
#include "../types/chars.h"               // char, string escape sequence handling.
#include "../types/vtypes.h"              // value types.
#include "../tokeniser/scope.h"           // scopes & stuff.
#include "../../common/_common.h"         // error handling & more.

// stdlib includes.
#include "stddef.h"                       // size_t, NULL.
#include "stdint.h"                       // integer types.
#include "stdbool.h"                      // boolean types.
#include "inttypes.h"                     // sscanf formats.

// ascii codes 0x00-0x1F, which are normally not displayed, are used for special name prefixes for certain variables in cth_name_ctxT.names.
enum{
  CTH_NAME_PFIX__TYPE = 0
};

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

// returns NULL on error, reports proper error when this happens so that underlying function doesn't have to.
static cth_variable__internalT* cth_value_decode(
  const char* restrict data, size_t s_data, size_t* const restrict s_raw, cth_nameT* const restrict type_raw,
  cth_error_ctxT* const restrict err, rcT* const restrict path, const cth_fposT line, const cth_fposT offset
){
  size_t s_format_specifier;                      // values can be terminated by format specifier; its length has to be accounted for.
  const cth_nameT type = cth_vtype(data, s_data, &s_format_specifier, err, path, line, offset);
  if(type == CTH_TYPE__NONE) 
    return NULL;
  *type_raw = type;

  int bytes_read, args_read;
  cth_variable__internalT* const restrict raw = malloc(sizeof(*raw));
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
      free(raw);
      return NULL;
    case CTH_TYPE__F32:
      *s_raw = sizeof(float);
      args_read = sscanf(data, "%f%n", &raw->f32, &bytes_read);
      break;
    case CTH_TYPE__F64:
      *s_raw = sizeof(double);
      args_read = sscanf(data, "%lf%n", &raw->f64, &bytes_read);
      break;
    case CTH_TYPE__U8:
    case CTH_TYPE__U16:
    case CTH_TYPE__U32:
    case CTH_TYPE__U64:
      *s_raw = sizeof(uint64_t);
      switch(encoding){
        case HEX:
          args_read = sscanf(data, "%" SCNx64 "%n", &raw->u64, &bytes_read); 
          if(negated) raw->u64 = -raw->u64;
          break;
        case DECIMAL:
          args_read = sscanf(data, "%" SCNu64 "%n", &raw->u64, &bytes_read);
          break;
        case OCTAL:
          args_read = sscanf(data, "%" SCNo64 "%n", &raw->u64, &bytes_read);
          if(negated) raw->u64 = -raw->u64;
          break;
        case BINARY:
          args_read = cth_value_decode__binary(data, s_data, &raw->u64, &bytes_read);
          if(negated) raw->u64 = -raw->u64;
          break;
      }
      break;
    case CTH_TYPE__I8:
    case CTH_TYPE__I16:
    case CTH_TYPE__I32:
    case CTH_TYPE__I64:
      *s_raw = sizeof(int64_t);
      switch(encoding){
        case HEX:
          args_read = sscanf(data, "%" SCNx64 "%n", &raw->i64, &bytes_read); 
          if(negated) raw->i64 = -raw->i64;
          break;
        case DECIMAL:
          args_read = sscanf(data, "%" SCNd64 "%n", &raw->i64, &bytes_read);
          break;
        case OCTAL:
          args_read = sscanf(data, "%" SCNo64 "%n", &raw->i64, &bytes_read);
          if(negated) raw->i64 = -raw->i64;
          break;
        case BINARY:
          args_read = cth_value_decode__binary(data, s_data, &raw->u64, &bytes_read);
          if(negated) raw->i64 = -raw->i64;
          break;
      }
      break;
    case CTH_TYPE__NONE:      // error reported by cth_vtype.
      free(raw);
      return NULL;
    default:                  // should not happen, very worrying if it does.
      cth_error_report__u32(CTH_ERROR__IMPOSSIBILITY, CTH_ERROR__COMPILER, path, line, offset, err, CTH_IMPOSSIBILITY__NAME_H_DEFAULT);
      free(raw);
      return NULL;
  }

  bytes_read += s_format_specifier;
  if(bytes_read != s_data)
    cth_error_report__u32(CTH_ERROR__VALUE_TOO_FEW_BYTES, CTH_ERROR__COMPILER, path, line, offset, err, bytes_read);
  return raw;
}

// searches for value, creates nameID if it doesn't find any.
// returns 0 on error.
static cth_nameT cth_value_add(
  const char* const restrict data, const size_t s_data, cth_tokeniserT* const restrict tokeniser,
  cth_error_ctxT* const restrict err, rcT* const restrict path, const cth_fposT line, const cth_fposT offset
){
  size_t s_raw;
  cth_nameT type_raw;
  cth_variable__internalT* const restrict raw = cth_value_decode(data, s_data, &s_raw, &type_raw, err, path, line, offset);
  if(!raw)
    return 0;

  vfastbst_str_nodeT** const restrict curr_node = vfastbst_str_put_half(&tokeniser->immed, (const char*)raw, s_raw);
  if(*curr_node){
    free(raw);
    return (*curr_node)->obj;
  }

  const cth_nameT id = cth_variable_name(tokeniser->variables);
  *curr_node = vfastbst_str_node_init((char*)raw, s_raw, id);
  cth_variableT* const restrict var = cth_variable_add(tokeniser->variables,
    id,
    (const char*)raw, s_raw, cth_typevar(type_raw),                 // typevar is safe to be used here because value_decode is guaranteed to return a normal integer type.
    CTH_VARSTATE__FREENAME | CTH_VARSTATE__IMMEDIATE
  );
  var->internal = *raw;

  return id;
}

static cth_nameT cth_char_add(
  const char* const restrict data, const size_t s_data, cth_tokeniserT* const restrict tokeniser,
  cth_error_ctxT* const restrict err, rcT* const restrict path, const cth_fposT line, const cth_fposT offset
){
  size_t s_raw;
  char* const restrict raw = malloc(1);
  *raw = cth_vtype_char(data, s_data, true, &s_raw, err, path, line, offset);

  vfastbst_str_nodeT** const restrict curr_node = vfastbst_str_put_half(&tokeniser->immed, raw, s_raw);
  if(*curr_node){
    free(raw);
    return (*curr_node)->obj;
  }

  const cth_nameT id = cth_variable_name(tokeniser->variables);
  *curr_node = vfastbst_str_node_init(raw, s_raw, id);
  cth_variableT* const restrict var = cth_variable_add(tokeniser->variables, id, raw, s_raw, cth_typevar(CTH_TYPE__CHAR8), CTH_VARSTATE__IMMEDIATE);
  var->internal.char8 = *raw;

  return id;
}

static cth_nameT cth_string_add(
  const char* const restrict data, const size_t s_data, cth_tokeniserT* const restrict tokeniser,
  cth_error_ctxT* const restrict err, rcT* const restrict path, const cth_fposT line, const cth_fposT offset
){
  size_t s_raw;
  char* const restrict raw = cth_vtype_string(data, s_data, &s_raw, err, path, line, offset);

  vfastbst_str_nodeT** const restrict curr_node = vfastbst_str_put_half(&tokeniser->immed, raw, s_raw);
  if(*curr_node){
    free(raw);
    return (*curr_node)->obj;
  }

  const cth_nameT id = cth_variable_name(tokeniser->variables);
  *curr_node = vfastbst_str_node_init(raw, s_raw, id);
  cth_variableT* const restrict var = cth_variable_add(tokeniser->variables, id, raw, s_raw, cth_typevar(CTH_TYPE__STRING), CTH_VARSTATE__IMMEDIATE);
  var->internal.string = rc_make(raw);

  return id;
}

#endif
