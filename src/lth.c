#define LTH_IMPLEMENTATION
#include "lth.h"

// TODO: add the very basics of compiler arg processing.

int main(const int argc, const char* argv[]){
  lth_outputT output = lth_compile(argc, argv);

  if(output){
    lth_output_print(output, stdout);
    lth_output_destroy(output);
  }
  return 0;
}
