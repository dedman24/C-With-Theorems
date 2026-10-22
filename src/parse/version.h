#ifndef LTH_PARSE_VERSION_H_INCLUDED
#define LTH_PARSE_VERSION_H_INCLUDED

#include "../common/ansi_colours.h"

// stdlib includes.
#include "stdio.h"              // printf.

#define LTH_VERSION_STR "1.0"

static void lth_parse__version(){
// so pretty :PPPPPP.
  puts(
    ANSI_COLOUR_GREEN
    "Lisp With Theorems version " LTH_VERSION_STR
    ANSI_COLOUR_RESET
    ", copyright 0xded "
    ANSI_COLOUR_CYAN
    "(https://github.com/dedman24)"
    ANSI_COLOUR_RESET
    "."
  );
}

#endif
