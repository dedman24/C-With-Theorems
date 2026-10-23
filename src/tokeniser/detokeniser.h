#ifndef CTH_DETOKENISER_H_INCLUDED
#define CTH_DETOKENISER_H_INCLUDED

#include "../common/_common.h"

// stdlib includes.
#include "stdio.h"          // fprintf & friends.

static void* cth_detokenise(
  const cth_tokT* const restrict src,
  const size_t elcnt
){
  fputs("the detokeniser has been deprecated; perhaps it will be readded in the future.\n", stderr);
  return NULL;
}

#endif

