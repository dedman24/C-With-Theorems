### Changelog 2026/11/12.
 - changed `type` field's type inside `variableT` from `nameT` to `variableT*`
 - tested declarations, now they work.

### pending changes.
 - arrays.
 - typedefs.

> `BEGIN snca;`

Broke some stuff, but it'll work again son.

> `END snca;`

### Current errors:
 - all strings & characters lead to errors.
 - path stuff in repl doesn't work, for most errors no path is printed.
 - hex, octal, binary values do not work.
 - no leading 0 making values octal.
 - something like `const char *x, *y` is currently not supported; perhaps generalise declarations.

### Things to add (in no particular order):
 - `void`, `void*` types.
 - structs, unions, enums.
 - function types, function calls.
 - working include stuff with `include` operator.
 - more ops (arithmetic & non).
 - '&&', '||', '^^' short-circuiting & not executing other stuff when it gets evaluated (within arbitrary expressions).
 - support for '++', '--' pre and post increment.
 - support for `op=` operators (like `+=`) with them being handled as syntax sugar.
 - looping constructs.
 - operator overrides.
 - `./cth "code"` and `./cth repl "code"` interpreting whatever is in the quotes.