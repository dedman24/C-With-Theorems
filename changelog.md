### Changelog 2026/11/27.
 - repl interprets only when scope is 0.
 - updated examples to use new theorem format.

### pending changes.
 - `**` pow operator.
 - arrays.
 - proper subtyping.
 - stricter type compatibility algorithm.
 - reusing old types in `cth_tokeniser_op__declaration_master_type`.
 - certain compound qualifiers (like `immut` & `const`) should be incompatible.

> `BEGIN snca;`

cth is proving to be such a chore I am unsure whether it was worth it or not to get this far.\

my real issue is that idk how to handle stuff like theorems in a clean syntactic way.\
say I wanted a theorem that `a + b == b + a`; how would I write this?\
the most c-like way, with types first, is:
>`a + b == b + a add_commutative(nat_t a, nat_t b);`

the issue is that a, b both need to be bound to `add_commutative`'s function scope, which clearly cannot be done if it preceeds it.\
another idea was to explicit the return type using a special `theorem` keyword, as:
>`theorem add_commutative(nat_t a, nat_t b) -> a + b == b + a;`

however, this is bad because:
 - it introduces a new syntax completely different than C.
 - it's not at all obvious that `a + b == b + a` is the return type (and that the return type isn't `theorem`).

>`theorem(a + b == b + a) add_commuative(nat_t a, nat_t b);`

this could work, it fits with C's style of doing stuff.\
ugh I really should learn all of type theory _now_ instead of larping knowledge of it. I didn't even code jack today.

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