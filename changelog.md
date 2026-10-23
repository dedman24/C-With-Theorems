### Changelog 2026/10/23.
 - cleaned up `tok.h`.
 - `./lth` by itself prints help info.
variable ctx properly initialises relevant symbols, like types, operators & bracket stuff.
LISP repl works, currently supports very few ops; more to be added.

### Current errors:
 - values terminating in standard rust-like integer suffixes do not work properly, lead to segfaults.
 - all strings lead to segfaults.

### Things to add:
 - more ops (arithmetic & non).
 - '&&', '||', '^^' short-circuiting & not executing other stuff when it gets evaluated (within arbitrary expressions).
 - first-class support for arrays.
 - array indexing not as (. a b) but as a.b directly, to aid readability.
 - path stuff to interpreter.
