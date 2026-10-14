#ifndef SATISFIABLE_COMMON_FPOST_H_INCLUDED
#define SATISFIABLE_COMMON_FPOST_H_INCLUDED

// fposT is a type used to represent positions within a file.
// for a full position within some file, two fposT-typed variables are needed: a line number & an offset within said line.

// stdlib includes.
#include "stdint.h"     // whatever unsigned int type is chosen to represent fposT.

typedef uint32_t satisfiable_fposT;

#endif

