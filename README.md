# C With Theorems.
'C With Theorems', also known as cth, is a simple dependent-typed programming language with syntax inspired from C. It is meant to be a reasonable combination of dependent typing and imperative languages; it tries to offer the _best of both worlds_.

## Building.
To build cth, one just has to execute `./build/buildall.sh cth`, which handles everything. I didn't use traditional build systems because I don't know how to use them (_cba to learn lol_) & a shell script works just fine.

## Usage.
To compile a program, one writes `cth [options] file.cth`. All output is written to stdout, save for errors which are written to stderr.

The following mutually exclusive options can be used:
* `help` ~ prints help string to stdout.
* `version` ~ prints current version & copyright info to stdout.
* `check` ~ type-checks code.
* `repl` ~ starts command-line REPL, useful for debugging programs.

If none of these options are explicited, all code is type-checked & compiled.

## Syntax.
Syntax is very simple, made to be visually unique & easy to read.\
**Functions** are written as `r_t foo(s_t src);` (C-style) or as `fn foo(s_t src) -> r_t` (cth-style, _differences do exist_ mainly regarding safety and aliasing rules; check out `meta/`). If multiple return types are specified, they are separated by commas.\
**Theorems** are written similarly, as `theorem foo(s_t src) -> condition;`. They can be thought of as functions taking some arguments & returning an object representing what the theorem proves. Check out `examples/naturals.cth` to see how one can prove addition over natural numbers is commutative in cth.\
I tried to give cth similar expressiveness as other dependent-typed theorem provers like Coq or Lean. I consider it successful in this regard, acthough more work is needed to ensure it can prove _all kinds of theorems_.

cth is **imperative**. This was done to make code faster.

## Why C With Theorems.
I really like C and C-like programming languages. I wanted a language _just like C_ so that all of my code is source-compatible with it, but with _a few things done differently_ (defer blocks, dependent types, better memory & type safety).\
C With Theorems is _exactly that_: ~~another C clone~~ a C with all the things I like about C, without the things I dislike, that is safer.\
"_just use (**C killer flavour of the month**)!_" ~ your language is either:
 - unsafe;
 - functional;
 - with a huge runtime;
 - dynamically typed;
 - without dependent typing;
 - a lisp variant;
 - rust (I don't like how it looks sry :PPP);

or a combination of the above, and that's enough of a reason for me to not use it.\
Not that you shouldn't use a language just because I said so, but those are languages I _don't like using_ that are often purported as successors to C. No, I won't use Haskell/ML/rust or whatever because they don't _feel like C_: save for rust they don't give one nearly as much control over the low-level machine as C does. btfo XDDD.

NOTE: I particularly dislike how basically all modern functional languages heavy runtimes; I'd use them if they didn't. No functional language currently is fast enough for one to write a full OS kernel in it, which is a project I'd like to try some day.

## Disclaimer
'C With Theorems' is a _hobbyist project_ I started to learn more about compilers, that I work on whenever I have time. I may make it seem something really cool and professionally developed in `meta/`, but it really _isn't_. It may _never even be finished_, so please get your hopes up.

_this project is **All bark, no bite**!_