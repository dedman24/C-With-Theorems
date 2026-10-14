#ifndef SATISFIABLE_PARSE_HELP_H_INCLUDED
#define SATISFIABLE_PARSE_HELP_H_INCLUDED

#include "version.h"

// stdlib includes.
#include "stdio.h"              // printf.

static void satisfiable_parse__help(const char* const restrict invoke_cmd){
    satisfiable_parse__version();

    printf("Usage: '%s [options] src_file.sat > dst_file.cnf'.\n", invoke_cmd);
    fputs(
        "Options:\n"
        "  help       ~ prints out help information.\n"
        "  version    ~ prints out version & copyright.\n"
        "  check      ~ checks program type validity, doesn't compile to CNF.\n"
        "  tokenise   ~ tokenises file, used for debugging.\n"
        "  detokenise ~ takes tokenised file & prints out original source code, used for debugging.\n",
        stdout
    );
}

#endif
