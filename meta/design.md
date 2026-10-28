# The aim of the C With Theorems programming language.
C With Theorems (written _cth_) is supposed to be a radical break from how the C programming language is evolving; it is supposed to be C if the C designers were willing to _add lots of new stuff_, while _keeping compatibility with old code_. **Indeed, cth is meant to be 100% compatible with all existing C code.**

C With Theorems _is not C_; the way it achieves compatibility while adding new stuff is _unorthodox_.\
Outright, the compiler acts differently when compiling cth code (ending in `.cth`/`.hth`) vs when compiling C code (assumed to be every other filetype). The differences are minor and amount to cth files including some C standard libraries and adding some new keywords (still available in C code by prefixing an underscore and making the first letter uppercase, like how `_Bool` still exists even when one doesn't `#include stdbool.h`). While minor, some of these differences are things I don't see the C committee adding in a billion years (better aliasing rules, namespaces).

C29 workgroup please email me just make C29 be cth without dependent typing everyone would love that everyone wants to see C with new features (except for people who actually use C, somehow they're the last ones to use them).

## How cth is designed.
cth usually exposes two ways of doings things:
 - _how it's usually done in C_, kept around for compatibility with C code; this works great for C because it doesn't try to be safe, but it wouldn't work in cth (which tries to be); therefore we also have:
 - _how it's meant to be done in cth_, which is safe and fast and perfect and pretty and so on.

The greatest example of this is with type declarations:
 - C types are declared using `typedef`s, and a type defined using `typedef` can be replaced (except in pointers) with the ones it is defined as. They're little better than just using `#define`.
 - cth types are defined as variables are, using the `type` type, as `type dst = src;`. In this example, `dst` is _always incompatible with `src`, and no implicit conversion between the two may happen.\
One can see all the differences in `meta/issues-with-C.md`.

## Why C.
C syntax might be the prettiest out of all languages, nothing really comes close to it.