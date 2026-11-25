#define CTH_IMPLEMENTATION
#include "cth.h"

// TODO: add the very basics of compiler arg processing.

int main(const int argc, const char* argv[]){
  return !cth_compile(argc, argv);
}
