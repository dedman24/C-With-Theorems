### Changelog 2026/11/21.
 - added round bracket handling in arithmetic expressions, no funcalls yet but adding them wouldn't be that hard.
 - added full if-operator support, no if statement yet.
 - something like `true if 200 else 300;` (if version of `true? 200: 300;`) doesn't work anymore.

### pending changes.
 - arrays.
 - full subtyping system.
 - stricter type compatibility.
 - reusing old types in `cth_tokeniser_op__declaration_master_type`.
 - certain compound qualifiers (like `immut` & `const`) should be incompatible.

> `BEGIN snca;`

I need to work harder.\
I truly believe cth can be something huge. after all, what other imperative language supports dependent typing & formal verification?\
_none_.

LORD help us all.

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
 - working include stuff with `include` operator.
 - more ops (arithmetic & non).
 - looping constructs.
 - operator overrides.
 - subtyping support fully replacing current type compatibility handling.
 - explicit type conversions added in AST at typechecking.