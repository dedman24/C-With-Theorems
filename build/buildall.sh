#!/bin/bash

# this script builds everything from scratch.
# bru bash is such an awful language anything would be better.
if [ $# -lt 1 ]; then
  echo "ERROR: too few arguments." >&2
elif [[ "$1" == "help" || "$1" == "--help" || "$1" == "-h" ]]; then
  echo "build script for satisfiability. usage is:"
  echo "$0 'path to final executable'"
else
  if [[ $# == 2 && "$2" == "debug" ]]; then
    gcc -fsanitize=address -g -std=c23 -o $1      \
      src/common/linking/vfastbst/vfastbst.c      \
      src/common/linking/vfastext/vfastext.c      \
      src/common/linking/vfastrht/vfastrht.c      \
      src/common/linking/vfastpath/vfastpath.c    \
      src/common/linking/vfasttask/vfasttask.c    \
      src/lth.c
  elif [[ $# == 2 && "$2" == "fast" ]]; then
    gcc -O1 -std=c23 -o $1                        \
      src/common/linking/vfastbst/vfastbst.c      \
      src/common/linking/vfastext/vfastext.c      \
      src/common/linking/vfastrht/vfastrht.c      \
      src/common/linking/vfastpath/vfastpath.c    \
      src/common/linking/vfasttask/vfasttask.c    \
      src/lth.c
  elif [[ $# == 2 ]]; then
    echo "ERROR: unknown build option."
  else
    gcc -flto -O3 -std=c23 -o $1                  \
      src/common/linking/vfastbst/vfastbst.c      \
      src/common/linking/vfastext/vfastext.c      \
      src/common/linking/vfastrht/vfastrht.c      \
      src/common/linking/vfastpath/vfastpath.c    \
      src/common/linking/vfasttask/vfasttask.c    \
      src/lth.c
  fi
fi
