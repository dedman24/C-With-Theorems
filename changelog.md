### Changelog 2026/11/24.
 - finally deprecated `cth_tokenise__single`.
 - reworked semicolon.
 - finished code block support.

### pending changes.
 - arrays.
 - proper subtyping.
 - stricter type compatibility algorithm.
 - reusing old types in `cth_tokeniser_op__declaration_master_type`.
 - certain compound qualifiers (like `immut` & `const`) should be incompatible.

> `BEGIN snca;`



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