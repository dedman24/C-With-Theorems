// vfastext is a simple extractable dynamic array written in C99.
// >what's wrong with vfastarr?
// one can't exactly 'extract' the array & free it using a standard call like free(); one _needs_ to track that it's an array & this becomes expensive quickly.
// therefore vfastext has a single vfastextT type that is trivially extractable with the right call.

#ifndef VFASTEXT_H_INCLUDED
#define VFASTEXT_H_INCLUDED

// stdlib includes.
#include "stddef.h"         // size_t, NULL.
#include "stdlib.h"         // malloc, free.
#include "string.h"         // memcpy.

typedef struct{
  void* arr;                // ptr to array itself.
  size_t size;              // elements array holds.
  size_t capacity;          // elements array _can_ hold.
} vfastextT;

void vfastext_destroy(vfastextT* const restrict ctx);
void vfastext_init(vfastextT* const restrict ctx, const size_t s_elem);
void vfastext_append(vfastextT* const restrict ctx, const void* const restrict elem, const size_t s_elem);
void* vfastext_extract(vfastextT* const restrict ctx);

#endif

#ifdef VFASTEXT_IMPLEMENTATION

void vfastext_destroy(vfastextT* const restrict ctx){
  free(ctx->arr);
}

void vfastext_init(vfastextT* const restrict ctx, const size_t s_elem){
  ctx->arr = malloc(s_elem*32);
  ctx->size = 0;
  ctx->capacity = s_elem*32;
}

static void vfastext_resize(vfastextT* const restrict ctx, const size_t amt){
  if(ctx->capacity*2 < amt) ctx->capacity += amt;
  else ctx->capacity *= 2;

  ctx->arr = realloc(ctx, ctx->capacity);
}

void vfastext_append(vfastextT* const restrict ctx, const void* const restrict elem, const size_t s_elem){
  if(ctx->size + s_elem > ctx->capacity)
    vfastext_resize(ctx, ctx->size + s_elem);
  memcpy((char*)ctx->arr + ctx->size, elem, s_elem);
  ctx->size += s_elem;
}

void* vfastext_extract(vfastextT* const restrict ctx){
  return ctx->arr;
}

#endif
