### Changelog 2026/11/17.
 - declarations without assignment used to segfault, not anymore.

### pending changes.
 - arrays.
 - typedefs.
 - reusing old types in `cth_tokeniser_op__declaration_master_type`.
 - certain compound qualifiers (like `immut` & `const`) shoul be incompatible.
 - rework how lvalues are differentiated from rvalues for assignment, so that expressions may _evaluate to lvalues_.

> `BEGIN snca;`

I think it'd be cool if instead of our current pipeline;
> `tokenisation -> interpreting`

We had an intermediate step that added type annotations and made compatibility simpler, like:
> `tokenisation -> typchecking -> interpreting`

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
 - looping constructs.
 - operator overrides.
 - subtyping support fully replacing current type compatibility handling.
 - explicit type conversions added in AST at typechecking.