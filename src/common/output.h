#ifndef LTH_COMMON_OUTPUT_H_INCLUDED
#define LTH_COMMON_OUTPUT_H_INCLUDED

// stdlib includes.
#include "stdio.h"                    // file handling.

typedef void* lth_outputT;
void lth_output_destroy(lth_outputT out);
static void lth_output_print(lth_outputT out, FILE* const restrict fd_out);


#ifdef LTH_IMPLEMENTATION

#include "linking/vfastarr.h"         // vfastarr type & handling.

static lth_outputT lth_output_init(){
  return vfastarr_init(1);
}

void lth_output_destroy(lth_outputT out){
  vfastarr_destroy(out);
}

static lth_outputT lth_output_add(lth_outputT output, void* const restrict data, const size_t len){
  return vfastarr_append(output, data, len);
}

static void lth_output_print(lth_outputT out, FILE* const restrict fd_out){
  fwrite(out, vfastarr_size(out), 1, fd_out);
}

#endif

#endif
