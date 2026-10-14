#!/bin/bash

# this script builds the program quickly by splitting it into its various parts.
if [[ "$1" == "all" ]]; then
  if [[ $# != 2 ]]; then
    echo "amount of arguments for option $1 is incorrect."
  fi

elif [[ "$1" == "some" ]]; then                     # compiles only minor libs.
  if [[ $# != 1 ]]; then
    echo "amount of arguments for option $1 is incorrect."
  fi

elif [[ "$1" == "main" ]]; then
  if [[ $# != 2 ]]; then
    echo "amount of arguments for option $1 is incorrect."
  fi

else
  echo "unknown option $1"
fi
