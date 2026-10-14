#ifndef SATISFIABLE_DETOKENISER_H_INCLUDED
#define SATISFIABLE_DETOKENISER_H_INCLUDED

#include "../common/_common.h"

// stdlib includes.
#include "stdio.h"          // fprintf & friends.

static void* satisfiable_detokenise(
  const satisfiable_tokT* const restrict src,
  const size_t elcnt
){
  char* restrict code = vfastarr_init(sizeof(*code));

  for(size_t i = 0; i < elcnt; i++){
    if(src[i].token == SATISFIABLE_TOK__EOF)
      break;
    if(src[i].token & SATISFIABLE_TOK__NAME_BIT)
      code = vfastarr_append(code, "NAME ", 5);
    else{
      if(satisfiable_tok__table[src[i].token] == NULL)
        fprintf(stderr, "HUGE ERROR!!!! %lu\n", src[i].token);

      code = vfastarr_append(code, satisfiable_tok__table[src[i].token], strlen(satisfiable_tok__table[src[i].token]));
      code = vfastarr_append(code, " ", 1);
      if(src[i].token == SATISFIABLE_TOK__SEMICOLON)
        code = vfastarr_append(code, "\n", 1);
    }
  }

  return code;
}

#endif

