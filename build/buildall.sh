#!/bin/bash

# this script builds everything from scratch.
# bru bash is such an awful language anything would be better.
if [ $# -lt 1 ]; then
  echo "ERROR: too few arguments." >&2
elif [[ "$1" == "help" || "$1" == "--help" || "$1" == "-h" ]]; then
  echo "build script for C With Theorems. usage is:"
  echo "    $0 final-executable"
  echo "options are:"
  echo "    'fast'    ~ compiles program as quickly as possible; resulting executable isn't that quick though."
  echo "    'small'   ~ compiles program so that it's as small as possible."
  echo "    'debug'   ~ produces debug build, which outputs error information on segfaults."
  echo "    'release' ~ compiles program with the highest optimisation settings possible."
  echo "if no option is specified, 'release' is used."
else
# see help options to see what each does.
  if [[ $# == 2 && "$2" == "fast" ]]; then
    gcc -O1 -std=c23 -o $1                        \
      src/common/linking/vfastbst/vfastbst.c      \
      src/common/linking/vfastext/vfastext.c      \
      src/common/linking/vfastrht/vfastrht.c      \
      src/common/linking/vfastpath/vfastpath.c    \
      src/common/linking/vfasttask/vfasttask.c    \
      src/cth.c
  elif [[ $# == 2 && "$2" == "small" ]]; then
    gcc -Os -o $1                                 \
      src/common/linking/vfastbst/vfastbst.c      \
      src/common/linking/vfastext/vfastext.c      \
      src/common/linking/vfastrht/vfastrht.c      \
      src/common/linking/vfastpath/vfastpath.c    \
      src/common/linking/vfasttask/vfasttask.c    \
      src/cth.c
  elif [[ $# == 2 && "$2" == "debug" ]]; then
    gcc -fsanitize=address -g -std=c23 -o $1      \
      src/common/linking/vfastbst/vfastbst.c      \
      src/common/linking/vfastext/vfastext.c      \
      src/common/linking/vfastrht/vfastrht.c      \
      src/common/linking/vfastpath/vfastpath.c    \
      src/common/linking/vfasttask/vfasttask.c    \
      src/cth.c
  elif [[ $# == 1 || "$2" == "release" ]]; then
    gcc -flto -O3 -std=c23 -o $1                  \
      src/common/linking/vfastbst/vfastbst.c      \
      src/common/linking/vfastext/vfastext.c      \
      src/common/linking/vfastrht/vfastrht.c      \
      src/common/linking/vfastpath/vfastpath.c    \
      src/common/linking/vfasttask/vfasttask.c    \
      src/cth.c
  else
    echo "ERROR: unknown build option."
  fi
fi
