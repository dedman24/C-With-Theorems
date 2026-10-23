### Changelog 2026/10/23.
 - added AST formation.

### Current errors:
 - values terminating in standard rust-like integer suffixes do not work properly, lead to segfaults.
 - all strings lead to segfaults.

### Things to add:
 - more ops (arithmetic & non).
 - '&&', '||', '^^' short-circuiting & not executing other stuff when it gets evaluated (within arbitrary expressions).
 - first-class support for arrays.
 - array indexing not as (. a b) but as a.b directly, to aid readability.
 - path stuff to interpreter.
