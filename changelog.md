### Changelog 2026/11/25.
 - added scoped tokeniser state.

### pending changes.
 - arrays.
 - proper subtyping.
 - stricter type compatibility algorithm.
 - reusing old types in `cth_tokeniser_op__declaration_master_type`.
 - certain compound qualifiers (like `immut` & `const`) should be incompatible.
 - scoped compiler state; currently I have not observed any issues caused by this, but they _will_ be a pain find & to debug.

> `BEGIN snca;`

the hardest days are thosse where I have as much code added as removed, as reported by `git push`.\
I want to get this over withhhh.

> `END snca;`

### Current errors:
 - all strings & characters lead to errors.
 - path stuff in interpreter doesn't work, for most errors no path is printed.
 - hex, octal, binary values do not work.
 - no leading 0 making values octal.

### Things to add (in no particular order):
 - full typechecking.
 - formalised typechecker.
 - `void`, `void*` types.
 - structs, unions, enums.
 - function types, function calls.
 - function polymorphism.
 - working include stuff with `include` operator.
 - more ops (arithmetic & non).
 - looping constructs.
 - operator overriding.
 - subtyping support fully replacing current type compatibility handling.
 - explicit type conversions added in AST at typechecking.