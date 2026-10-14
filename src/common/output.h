#ifndef SATISFIABLE_COMMON_OUTPUT_H_INCLUDED
#define SATISFIABLE_COMMON_OUTPUT_H_INCLUDED

// stdlib includes.
#include "stdio.h"                    // file handling.

typedef void* satisfiable_outputT;
void satisfiable_output_destroy(satisfiable_outputT out);
static void satisfiable_output_print(satisfiable_outputT out, FILE* const restrict fd_out);


#ifdef SATISFIABLE_IMPLEMENTATION

#include "linking/vfastarr.h"         // vfastarr type & handling.

static satisfiable_outputT satisfiable_output_init(){
  return vfastarr_init(1);
}

void satisfiable_output_destroy(satisfiable_outputT out){
  vfastarr_destroy(out);
}

static satisfiable_outputT satisfiable_output_add(satisfiable_outputT output, void* const restrict data, const size_t len){
  return vfastarr_append(output, data, len);
}

static void satisfiable_output_print(satisfiable_outputT out, FILE* const restrict fd_out){
  fwrite(out, vfastarr_size(out), 1, fd_out);
}

#endif

#endif
