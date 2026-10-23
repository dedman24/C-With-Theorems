# C With Theorems.
'C With Theorems', also known as cth, is a simple dependent-typed programming language with syntax inspired from C. It is meant to be the simplest possible dependent typed programming language one can reasonably implement.

## Building.
To build cth, one just has to execute `./build/buildall.sh cth`, which handles everything. I didn't use traditional build systems because I don't know how to use them (_cba to learn lol_) & a shell script works just fine.

## Usage.
To compile a program, one writes `cth [options] file.cth`. All output is written to stdout, save for errors which are written to stderr.

The following mutually exclusive options can be used:
* `help` ~ prints help string to stdout.
* `version` ~ prints current version & copyright info to stdout.
* `check` ~ type-checks code.
* `repl` ~ starts command-line REPL, useful for debugging programs.
If no options are explicited, all code is type-checked & compiled.

## Syntax.
Syntax is very simple, made to be visually unique & easy to read.\
**Functions** are written as `r_t foo(s_t src);`. If multiple return types are specified, they are separated by commas.\
**Theorems** are written similarly, as `theorem foo(s_t src);`. They can be thought of as functions taking some arguments & returning an object representing what the theorem proves.\
I tried to give cth similar expressiveness as other dependent-typed theorem provers like Coq or Lean. I consider it successful in this regard, acthough more work is needed to ensure it can prove _all kinds of theorems_.

cth is **imperative**. This was done to make code faster.

## Why C With Theorems.
I really like C and C-like programming languages. I wanted a language _just like C_ so that all of my code is source-compatible, but with _a few things done differently_ (defer statements, dependent types, better safety).\