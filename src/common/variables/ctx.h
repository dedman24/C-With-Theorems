#ifndef CTH_COMMON_VARIABLES_CTX_H_INCLUDED
#define CTH_COMMON_VARIABLES_CTX_H_INCLUDED

#include "../tok.h"                         // nameT & frens.
#include "../linking/rc.h"                  // reference-counted strings.
#include "../linking/vfastarr.h"            // vfastarr stuff.
#include "../../interpreter/ast.h"          // abstract syntax tree.
#include "../linking/vfastrht/vfastrht.h"   // vfastrhtT type & functions.

// stdlib includes.
#include "uchar.h"                // wide char types.

// POSIX includes.
#include "sys/types.h"            // ssize_t.

typedef struct{
  cth_astT* tree;
  uint32_t rc;                    // reference count.
} cth_type__internalT;

typedef enum: uint8_t{
  CTH_VARSTATE__NONE      = 0,
  CTH_VARSTATE__FREEALL   = 1<<0,
  CTH_VARSTATE__FREENAME  = 1<<1,
  CTH_VARSTATE__RVALUE    = 1<<2,
  CTH_VARSTATE__ASSIGNED  = 1<<3,
  CTH_VARSTATE__IMMEDIATE = 1<<4,
} cth_variable_stateT;

typedef union{
  char char8;
  char16_t char16;
  char32_t char32;
  rcT* string;

  int8_t i8;
  int16_t i16;
  int32_t i32;
  int64_t i64;

  uint8_t u8;
  uint16_t u16;
  uint32_t u32;
  uint64_t u64;

  float f32;
  double f64;

  char c;
  signed char sc;
  unsigned char uc;
  short sh;
  unsigned short ush;
  int i;
  unsigned int ui;
  long l;
  unsigned long ul;
  long long ll;
  unsigned long long ull;
// C floats.
  float f;
  double d;
  long double ld;

  bool boolean;
  uintptr_t ptr;

  cth_type__internalT* type;
} cth_variable__internalT;                // internal representation/actual contents of variable.

typedef struct cth_variableS{
  const char* name;                       // string name of variable.
  cth_variable__internalT internal;       // internal contents of variable.
  size_t s_name;                          // length of name in bytes.
  struct cth_variableS* type;             // ptr to variable holding type.
  cth_variable_stateT state;              // internal state of variable.
} cth_variableT;

typedef struct{
  vfastrhtT* correspondences;
  cth_nameT id_name;
} cth_variable_ctxT;

static cth_variableT* cth_variable_add(
  cth_variable_ctxT* const restrict ctx,
  const cth_nameT id, const char* const restrict name, const size_t s_name,
  cth_variableT* const type, const cth_variable_stateT state
);

#endif
