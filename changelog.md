### Changelog 2026/10/24.
 - renamed `examples/program0.cth` to `examples/naturals.cth`, made it more legible.
 - added AST stuff somewhat.
 - updated README.md.
 - now compiles, but idk if it works rofl.

### Current errors:
 - values terminating in standard rust-like integer suffixes do not work properly, lead to segfaults.
 - all strings lead to segfaults.

### Things to add:
 - working include stuff.
 - more ops (arithmetic & non).
 - '&&', '||', '^^' short-circuiting & not executing other stuff when it gets evaluated (within arbitrary expressions).
 - first-class support for arrays.
 - array indexing not as (. a b) but as a.b directly, to aid readability.
 - path stuff to interpreter.
