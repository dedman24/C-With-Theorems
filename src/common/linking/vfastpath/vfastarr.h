#ifndef VFASTARR_H_INCLUDED
#define VFASTARR_H_INCLUDED

// vfastarr is a simple dynamic array that does cool data shadowing.

// stdlib includes.
#include "stddef.h"             // for size_t.
#include "stdlib.h"             // for malloc, realloc.
#include "string.h"             // for memcpy.

// functions.
size_t vfastarr_size(void* const restrict arr);                                                   // gets size of array.
size_t vfastarr_elcnt(void* const restrict arr, const size_t s_elem);                             // gets No. of elements within array.
void* vfastarr_append(void* restrict arr, const void* const restrict elem, const size_t s_elem);  // appends element to array.
void* vfastarr_extend(void* restrict arr, const size_t c_elem, const size_t s_elem);              // extends array by cnt elements.
void vfastarr_pop(void* restrict arr, void* const restrict elem, const size_t s_elem);            // pops element from end of array.
void vfastarr_remove(void* restrict arr, const size_t s_elem);                                    // removes element from end of array.
void* vfastarr_init(const size_t s_elem);                                                         // initialises array.
void vfastarr_destroy(void* const arr);                                                           // destroys array.

#endif

#ifdef VFASTARR_IMPLEMENTATION

typedef struct{
  size_t size;                  // how many elements it effectively holds.
  size_t capacity;              // how many elements it can hold.
} vfastarr_hdrT;

// returns size of array in No. of bytes the array actually holds.
size_t vfastarr_size(void* const restrict arr){
  vfastarr_hdrT* const restrict hdr = (vfastarr_hdrT*)arr - 1;
  return hdr->size;
}

size_t vfastarr_elcnt(void* restrict arr, const size_t s_elem){
  return vfastarr_size(arr)/s_elem;
}

static vfastarr_hdrT* vfastarr_resize(vfastarr_hdrT* restrict hdr){
  hdr->capacity *= 2;

  hdr = realloc(hdr, sizeof(*hdr) + hdr->capacity);
  return hdr;
}

static vfastarr_hdrT* vfastarr_resize__explicit(vfastarr_hdrT* restrict hdr, const size_t amt){
  if(hdr->capacity*2 < amt) hdr->capacity += amt;
  else hdr->capacity *= 2;

  hdr = realloc(hdr, sizeof(*hdr) + hdr->capacity);
  return hdr;
}

// appends element to array, returns updated array.
void* vfastarr_append(void* restrict arr, const void* const restrict elem, const size_t s_elem){
  vfastarr_hdrT* restrict hdr = (vfastarr_hdrT*)arr - 1;
  if(hdr->size + s_elem > hdr->capacity){
    hdr = vfastarr_resize(hdr);
    arr = (void*)(hdr + 1);
  }
  memcpy((char*)arr + hdr->size, elem, s_elem);
  hdr->size += s_elem;
  return arr;
}

void* vfastarr_extend(void* restrict arr, const size_t c_elem, const size_t s_elem){
  vfastarr_hdrT* restrict hdr = (vfastarr_hdrT*)arr - 1;
  if(hdr->size + c_elem*s_elem > hdr->capacity){
    hdr = vfastarr_resize__explicit(hdr, c_elem*s_elem);
    arr = (void*)(hdr + 1);
  }
  hdr->size += c_elem*s_elem;
  return arr;
}

void vfastarr_pop(void* restrict arr, void* const restrict elem, const size_t s_elem){
  vfastarr_hdrT* const hdr = (vfastarr_hdrT*)arr - 1;

  if(hdr->size < s_elem){
    memset(elem, 0, s_elem);
    return;
  }
  hdr->size -= s_elem;
  memcpy(elem, (char*)arr + hdr->size, s_elem);
}

void vfastarr_remove(void* restrict arr, const size_t s_elem){
  vfastarr_hdrT* const hdr = (vfastarr_hdrT*)arr - 1;

  if(hdr->size < s_elem) return;
  hdr->size -= s_elem;
}

void* vfastarr_init(const size_t s_elem){
  vfastarr_hdrT* const restrict hdr = malloc(sizeof(vfastarr_hdrT) + 16*s_elem);
  hdr->size = 0;
  hdr->capacity = 16*s_elem;
  return (void*)(hdr + 1);
}

void vfastarr_destroy(void* const arr){
  vfastarr_hdrT* const hdr = (vfastarr_hdrT*)arr - 1;
  free(hdr);
}

#endif
