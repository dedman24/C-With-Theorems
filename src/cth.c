#define CTH_IMPLEMENTATION
#include "cth.h"

// TODO: add the very basics of compiler arg processing.

int main(const int argc, const char* argv[]){
  cth_outputT output = cth_compile(argc, argv);

  if(output){
    cth_output_print(output, stdout);
    cth_output_destroy(output);
  }
  return 0;
}
