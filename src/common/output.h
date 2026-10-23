#ifndef CTH_COMMON_OUTPUT_H_INCLUDED
#define CTH_COMMON_OUTPUT_H_INCLUDED

// stdlib includes.
#include "stdio.h"                    // file handling.

typedef void* cth_outputT;
void cth_output_destroy(cth_outputT out);
static void cth_output_print(cth_outputT out, FILE* const restrict fd_out);


#ifdef CTH_IMPLEMENTATION

#include "linking/vfastarr.h"         // vfastarr type & handling.

static cth_outputT cth_output_init(){
  return vfastarr_init(1);
}

void cth_output_destroy(cth_outputT out){
  vfastarr_destroy(out);
}

static cth_outputT cth_output_add(cth_outputT output, void* const restrict data, const size_t len){
  return vfastarr_append(output, data, len);
}

static void cth_output_print(cth_outputT out, FILE* const restrict fd_out){
  fwrite(out, vfastarr_size(out), 1, fd_out);
}

#endif

#endif
