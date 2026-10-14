#ifndef SATISFIABLE_COMMON_INCLUDET_H_INCLUDED
#define SATISFIABLE_COMMON_INCLUDET_H_INCLUDED

// 'IncludeT' is a type th

#include "tok.h"

// stdlib includes.
#include "stdlib.h"             // calloc, free.

typedef satisfiable_tokT* _Atomic satisfiable_includeT;

static satisfiable_includeT* satisfiable_include_init(){
  return calloc(1, sizeof(satisfiable_includeT));
}

static void satisfiable_include_destroy(satisfiable_includeT* const restrict obj){
  free(obj);
}

#endif
