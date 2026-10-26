### Changelog 2026/10/26.
 - expressions like `1+2;` are not parsed as `2+2;` anymore.

### Current errors:
 - values terminating in rust-like integer suffixes do not work properly, lead to segfaults.
 - all strings lead to segfaults.

### Things to add:
 - working include stuff.
 - more ops (arithmetic & non).
 - '&&', '||', '^^' short-circuiting & not executing other stuff when it gets evaluated (within arbitrary expressions).
 - first-class support for arrays.
 - path stuff to interpreter.
 - better default types; current ones _suck_.
