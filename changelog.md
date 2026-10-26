### Changelog 2026/10/26.
 - updated build script, now supports `small` option.
 - added `meta/`.
 - added disclaimer to `README.md`.
 - removed `in` keyword.
 - added proper handling of variableT in interpreter; now does not leak memory.
 - readded working repl, can parse simple expressions like `1+2;`.

### Current errors:
 - expressions like `1+2;` is evaluated as if it were `2+2;` (**really huge error fix this ASAP!**).
 - values terminating in rust-like integer suffixes do not work properly, lead to segfaults.
 - all strings lead to segfaults.

### Things to add:
 - working include stuff.
 - more ops (arithmetic & non).
 - '&&', '||', '^^' short-circuiting & not executing other stuff when it gets evaluated (within arbitrary expressions).
 - first-class support for arrays.
 - path stuff to interpreter.
 - better default types; current ones _suck_.
