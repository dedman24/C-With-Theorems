### Changelog 2026/10/whatever.
 - prettified `satisfiable_repl__result`.
 - '+' concatenates strings.
 - added '-', '*', '/', '%', '&', '|', '^', unary '~'.
 - renamed from Satisfiable to Lisp With Theorems (lth).
 - updated README.md.
added LISP repl.

### Current errors:
 - values terminating in standard rust-like integer suffixes do not work properly, lead to segfaults.
 - all strings lead to segfaults.
 - stuff like '1+1' doesn't evaluate correctly, when both names are the same variable.

### Things to add:
 - more ops.
 - '&&', '||', '^^' short-circuiting & not executing other stuff when it gets evaluated (within arbitrary expressions).
 - first-class support for listsarrays & other types, perhaps even user-generated types to make safety simpler.
 - more complex types, ultimately giving us dependent type support.
 - multiple dispatch.
 - path stuff to interpreter.
