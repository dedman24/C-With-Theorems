### Changelog 2026/11/whatever.
 - support for printing values of type `type`.
 - something like `i32 immut* immut` caused a bunch of errors for some reason; now fixed.
 - improper construction of type names would occasionally cause use-after-free errors, now works properly.


### Pending changes.
 - something like `i32* x = NULL;` didn't work, now does.
 - support for multiple definitions, like `int x = 1, y = 2;` handled the same way it is in C.
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

### Things to add (in no particular order):
 - `void`, `void*` types.
 - structs, unions, enums.
 - function types, function calls.
 - working include stuff with `include` operator.
 - more ops (arithmetic & non).
 - '&&', '||', '^^' short-circuiting & not executing other stuff when it gets evaluated (within arbitrary expressions).
 - support for '++', '--' pre and post increment
 - looping constructs.
 - operator overrides.