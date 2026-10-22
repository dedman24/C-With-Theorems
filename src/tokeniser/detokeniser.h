#ifndef LTH_DETOKENISER_H_INCLUDED
#define LTH_DETOKENISER_H_INCLUDED

#include "../common/_common.h"

// stdlib includes.
#include "stdio.h"          // fprintf & friends.

static void* lth_detokenise(
  const lth_tokT* const restrict src,
  const size_t elcnt
){
  fputs("the detokeniser has been deprecated; perhaps it will be readded in the future.\n", stderr);
  return NULL;
}

#endif

