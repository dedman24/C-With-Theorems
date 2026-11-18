### Changelog 2026/11/19.
 - reworked how lvalues are differentiated from rvalues for assignment, expressions now may _evaluate to lvalues_.
 - added `cth_variable_clone`.
 - fixed `::` typechecking operator.
 - updated tokens list.
 - added C-style typedefs.

### pending changes.
 - arrays.
 - stricter type compatibility.
 - reusing old types in `cth_tokeniser_op__declaration_master_type`.
 - certain compound qualifiers (like `immut` & `const`) should be incompatible.

> `BEGIN snca;`

dyer lane, we gon' hunt you down...

I feel like anything over 3 commits/day is either larp or AI.

while typedefs are interesting, we already support declarations of types with the `type` type, so that smth like `type i32_ptr = i32*;` works great.\
`typedef` exists for _historical reasons_ (ugh a new programming language with technical debt, and it isn't even finished?!).

> `END snca;`

### Current errors:
 - all strings & characters lead to errors.
 - path stuff in repl doesn't work, for most errors no path is printed.
 - hex, octal, binary values do not work.
 - no leading 0 making values octal.

### Things to add (in no particular order):
 - full typechecking.
 - formalised typechecker.
 - `void`, `void*` types.
 - structs, unions, enums.
 - function types, function calls.
 - working include stuff with `include` operator.
 - more ops (arithmetic & non).
 - looping constructs.
 - operator overrides.
 - subtyping support fully replacing current type compatibility handling.
 - explicit type conversions added in AST at typechecking.