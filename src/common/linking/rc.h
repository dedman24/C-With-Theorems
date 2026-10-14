// reference-counted strings.
// linked together with vfastrht.
// why doesn't C have this already? we need a larger C stdlib.
// (insert 500th language that tries to be C-but-better).

#ifndef RC_H_INCLUDED
#define RC_H_INCLUDED

// stdlib includes.
#include "stdint.h"         // integer types.
#include "stdlib.h"         // malloc, free.

#if !defined(RC_REFERENCE_BOUND) || RC_REFERENCE_BOUND < 256
typedef uint8_t rc_refT;
#elif RC_REFERENCE_BOUND < 65535
typedef uint16_t rc_refT;
#else
typedef uint32_t rc_refT;
#endif

typedef struct{
  const char* str;
  rc_refT rcount;
} rcT;

rcT* rc_make(const char* const restrict str);
rcT* rc_own(rcT* const restrict rc);
void rc_disown(rcT* const restrict rc);

#endif


#ifdef RC_IMPLEMENTATION

rcT* rc_make(const char* const restrict str){
  rcT* const restrict rc = malloc(sizeof(*rc));

  rc->str = str;
  rc->rcount = 1;

  return rc;
}

rcT* rc_own(rcT* const restrict rc){
  rc->rcount++;
  return rc;
}

void rc_disown(rcT* const restrict rc){
  rc->rcount--;
  if(rc->rcount == 0) free(rc);
}

#endif

