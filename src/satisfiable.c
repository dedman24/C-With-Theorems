#define SATISFIABLE_IMPLEMENTATION
#include "satisfiable.h"

// TODO: add the very basics of compiler arg processing.

int main(const int argc, const char* argv[]){
  satisfiable_outputT output = satisfiable_compile(argc, argv);

  if(output){
    satisfiable_output_print(output, stdout);
    satisfiable_output_destroy(output);
  }
  return 0;
}
