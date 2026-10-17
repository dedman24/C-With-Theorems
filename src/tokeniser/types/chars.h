#ifndef SATISFIABLE_TOKENISER_TYPES_CHARS_H_INCLUDED
#define SATISFIABLE_TOKENISER_TYPES_CHARS_H_INCLUDED

#include "../../common/_common.h"

// stdlib includes.
#include "stddef.h"       // size_t.
#include "stdbool.h"      // bool, true, false.

// basic function which reads 1 character & returns it, how many bytes were consumed & so on.
// ischar ~ whether we're processing a single character (set to true) or a string (set to false).
static char satisfiable_vtype_char(
  const char* const restrict data, const size_t s_data, const bool isquote, int* const restrict bytes_read,
  satisfiable_error_ctxT* const restrict err, rcT* const restrict path, const satisfiable_fposT line, const satisfiable_fposT offset
){
// cannot be not terminated by quote, unless terminated by \0, but in case we decide to add other terminating characters this will work.
  if(isquote && data[s_data] != '\''){
    satisfiable_error_report(SATISFIABLE_ERROR__VALUE_CHAR_UNFINISHED, SATISFIABLE_ERROR__COMPILER, path, line, offset, err);
    return 0;
  }
  if(data[0] != '\\'){
    if(bytes_read) *bytes_read = 1;
    return data[0];
  }
// no easier way of doing this...
  switch(data[1]){
    case 'a': if(bytes_read) *bytes_read = 2; return '\a';
    case 'b': if(bytes_read) *bytes_read = 2; return '\b';
    case 'e': if(bytes_read) *bytes_read = 2; return '\e';
    case 'f': if(bytes_read) *bytes_read = 2; return '\f';
    case 'n': if(bytes_read) *bytes_read = 2; return '\n';
    case 'r': if(bytes_read) *bytes_read = 2; return '\r';
    case 't': if(bytes_read) *bytes_read = 2; return '\t';
    case 'v': if(bytes_read) *bytes_read = 2; return '\v';
    case '\\': if(bytes_read) *bytes_read = 2; return '\\';
    case '\'': if(bytes_read) *bytes_read = 2; return '\'';
    case '\"': if(bytes_read) *bytes_read = 2; return '\"';
    case '\?': if(bytes_read) *bytes_read = 2; return '\?';   // historical C nonsense. so what? I still like it<3333333333.
    case 'o':{                        // arbitrary octal value of the format \o....
      int value, s_value;
      const int c_args = sscanf(data + 2, "%o%n", &value, &s_value);
      if(!c_args || s_value != s_data - 3 /* final quote & first 2 chars we skipped over */ && isquote)
        satisfiable_error_report(SATISFIABLE_ERROR__VALUE_CHAR_TOO_SHORT, SATISFIABLE_ERROR__COMPILER, path, line, offset, err);
      if(bytes_read) *bytes_read = s_value;
      return value;
    }
    case 'x':{                        // arbitrary hex value of the format \x....
      int value, s_value;
      const int c_args = sscanf(data + 2, "%x%n", &value, &s_value);
      if(!c_args || s_value != s_data - 3 /* final quote & first 2 chars we skipped over */ && isquote)
        satisfiable_error_report(SATISFIABLE_ERROR__VALUE_CHAR_TOO_SHORT, SATISFIABLE_ERROR__COMPILER, path, line, offset, err);
      if(bytes_read) *bytes_read = s_value;
      return value;
    }
    default:                          // arbitrary decimal value of the format \... or an error if not a digit.
      if(data[1] < '0' || data[1] > '9'){
        satisfiable_error_report(SATISFIABLE_ERROR__VALUE_UNKNOWN_ESCAPE_SEQUENCE, SATISFIABLE_ERROR__COMPILER, path, line, offset, err);
        if(bytes_read) *bytes_read = 2;
        return 0;
      }{
      int value, s_value;
      const int c_args = sscanf(data + 2, "%d%n", &value, &s_value);
      if(!c_args || s_value != s_data - 3 /* final quote & first 2 chars we skipped over */ && isquote)
        satisfiable_error_report(SATISFIABLE_ERROR__VALUE_CHAR_TOO_SHORT, SATISFIABLE_ERROR__COMPILER, path, line, offset, err);
      if(bytes_read) *bytes_read = s_value;
      return value;
      }
  }
} 

static char* satisfiable_vtype_string(
  const char* const restrict data, const size_t s_data, size_t* const restrict s_raw,
  satisfiable_error_ctxT* const restrict err, rcT* const restrict path, const satisfiable_fposT line, const satisfiable_fposT offset
){
  vfastextT arr; vfastext_init(&arr, sizeof(char));
  if(data[s_data] != '"')             // we still go on, returning NULL is far more harmful.
    satisfiable_error_report(SATISFIABLE_ERROR__VALUE_STRING_UNFINISHED, SATISFIABLE_ERROR__COMPILER, path, line, offset, err);
  for(size_t i = 0; i < s_data;){
    int bytes_read;
    const char c = satisfiable_vtype_char(data + i, s_data - i, false, &bytes_read, err, path, line, offset);
    vfastext_append(&arr, &c, sizeof(c));
    i += bytes_read;
  }

  *s_raw = arr.size;
  return vfastext_extract(&arr);
}

#endif
