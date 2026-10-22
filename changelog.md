### Changelog 2026/10/23.
 - updated README.md
 - updated tokeniser.

### Current errors:
 - LISP repl is unsupported, 
 - values terminating in standard rust-like integer suffixes do not work properly, lead to segfaults.
 - all strings lead to segfaults.

### Things to add:
 - more ops.
 - '&&', '||', '^^' short-circuiting & not executing other stuff when it gets evaluated (within arbitrary expressions).
 - first-class support for arrays.
 - path stuff to interpreter.
