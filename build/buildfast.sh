#!/bin/bash

# this script builds the program quickly by splitting it into its various parts.
if [[ "$1" == "all" ]]; then
  if [[ $# != 2 ]]; then
    echo "amount of arguments for option $1 is incorrect."
  fi

  ./buildall.sh $2
elif [[ "$1" == "some" ]]; then                     # compiles only minor libs.
  if [[ $# != 1 ]]; then
    echo "amount of arguments for option $1 is incorrect."
  fi

  gcc -flto -O3 -c src/common/linking/vfastbst/vfastbst.c src/common/linking/vfastpath/vfastpath.c src/common/linking/vfasttask/vfasttask.c
elif [[ "$1" == "main" ]]; then
  if [[ $# != 2 ]]; then
    echo "amount of arguments for option $1 is incorrect."
  fi

  gcc -flto -O3 -c src/satisfiable.c
  gcc -flto -O3 satisfiable.o vfastpath.o vfasttask.o vfastbst.o -o $2
else
  echo "unknown option $1"
fi
