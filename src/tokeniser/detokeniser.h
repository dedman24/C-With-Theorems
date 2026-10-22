#ifndef LTH_DETOKENISER_H_INCLUDED
#define LTH_DETOKENISER_H_INCLUDED

#include "../common/_common.h"

// stdlib includes.
#include "stdio.h"          // fprintf & friends.

static void* lth_detokenise(
  const lth_tokT* const restrict src,
  const size_t elcnt
){
  char* restrict code = vfastarr_init(sizeof(*code));

  for(size_t i = 0; i < elcnt; i++){
    if(src[i].token == LTH_TOK__EOF)
      break;
    if(src[i].token & LTH_TOK__NAME_BIT)
      code = vfastarr_append(code, "NAME ", 5);
    else{
      if(lth_tok__table[src[i].token] == NULL)
        fprintf(stderr, "HUGE ERROR!!!! %lu\n", src[i].token);

      code = vfastarr_append(code, lth_tok__table[src[i].token], strlen(lth_tok__table[src[i].token]));
      code = vfastarr_append(code, " ", 1);
      if(src[i].token == LTH_TOK__SEMICOLON)
        code = vfastarr_append(code, "\n", 1);
    }
  }

  return code;
}

#endif

