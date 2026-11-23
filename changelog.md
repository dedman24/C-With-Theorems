### Changelog 2026/11/23.
 - added proof in cth of sqrt(2) being irrational in `examples/sqrt2.cth`.
 - reworked variable output.
 - added statement/code block support.
 - started work on better tokeniser scoping model.
 - added `!:` token to express an element not being of some type.
 - removed name ctx, added what remained as separate thing in tokeniser.

### pending changes.
 - rework semicolon.
 - add code block support to `if` statements and in generic expressions (like in rust).
 - arrays.
 - proper subtyping.
 - stricter type compatibility algorithm.
 - reusing old types in `cth_tokeniser_op__declaration_master_type`.
 - certain compound qualifiers (like `immut` & `const`) should be incompatible.

> `BEGIN snca;`

while not on cth, I'm actually working pretty hard.\
I want to keep this up

I broke my github streak TwT...\
my fault since I couldn't add scopes yesterday but scopes are quite a lot of stuff already.

I really want to finish this by tmrw. I'll work hard.

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