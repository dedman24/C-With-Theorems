#ifndef ANSI_COLOURS_H_INCLUDED
#define ANSI_COLOURS_H_INCLUDED

// holds terminal ansi colour stuff.
// NOTE: ncurses might be more portable.

// stdlib includes.
#include "stdio.h"        // so the file using this lib doesn't have to include it on its own.

#define ANSI_COLOUR_RED     "\x1b[31m"
#define ANSI_COLOUR_GREEN   "\x1b[32m"
#define ANSI_COLOUR_YELLOW  "\x1b[33m"
#define ANSI_COLOUR_BLUE    "\x1b[34m"
#define ANSI_COLOUR_MAGENTA "\x1b[35m"
#define ANSI_COLOUR_CYAN    "\x1b[36m"
#define ANSI_COLOUR_RESET   "\x1b[0m"

#endif
