### Changelog 2026/11/14::2.
 - make tokeniser add type annotations to declared variables.
 - add declarations supporting smth like `const char* x = NULL, y = 10;`.

### pending changes.
 - arrays.
 - typedefs.
 - compound qualifiers (like `static const`) not segfaulting.
 - add reusing old types in `cth_tokeniser_op__declaration_master_type`.

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