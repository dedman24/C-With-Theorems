### Changelog 2026/11/14.
 - `immut`, `const`, `constexpr` values could be reassigned, not anymore.
 - `./cth repl 'code'` interprets whatever is in quotes.
 - added `typeof` unary operator.
 - `&&`, `||`, `^^` short-circuit & they do not execute other stuff when it gets evaluated (within arbitrary expressions).
 - fixed memleak with returning `NULL` without cleaning up in all binary ops.

### pending changes.
 - arrays.
 - add typedefs.
 - make tokeniser add type annotations to declared variables.
 - add declarations supporting smth like `const char* x = NULL, y = 10;`.

> `BEGIN snca;`

> `END snca;`

### Current errors:
 - all strings & characters lead to errors.
 - path stuff in repl doesn't work, for most errors no path is printed.
 - hex, octal, binary values do not work.
 - no leading 0 making values octal.

### Things to add (in no particular order):
 - full typechecking.
 - `void`, `void*` types.
 - structs, unions, enums.
 - function types, function calls.
 - working include stuff with `include` operator.
 - more ops (arithmetic & non).
 - support for `op=` operators (like `+=`) with them being handled as syntax sugar.
 - looping constructs.
 - operator overrides.
 - subtyping.