### Changelog 2026/11/26.
 - added `?:` as `CTH_TOK__ISTYPE` in tokeniser and interpreter.
 - fixed `if`, ternary-`if` statements.
 - finally finished scope support.

### pending changes.
 - working scopes in expressions.
 - arrays.
 - proper subtyping.
 - stricter type compatibility algorithm.
 - make `cth` return 1 if it produces errors.
 - reusing old types in `cth_tokeniser_op__declaration_master_type`.
 - certain compound qualifiers (like `immut` & `const`) should be incompatible.

> `BEGIN snca;`

I hope better testing infrastructure leads to quicker bugcatching.

with `CTH_TOK__ISTYPE`, cth has 81 tokens (without counting types & other keywords). this is quite a lot.\
from here onwards, I want to avoid the trend set by C++ and rust of making code absolutely unreadable by introducing a trillion operators.\
I think I'll limit myself to `!` being a qualifier denoting `noignore`; everything else is a verbal qualifier.

something like `immut qux_t(x)&! foo(immut qux_t(y)&! bar)` is read as a function taking a param `bar` of type `qux_t(y) immut reference` and returning a `qux_t(x) immut reference`, where the input arg is _acquired_ by the function & may never be used again by the caller, and the result _must not be ignored_.\
this is fine when explained like this, and `immut qux_t(y)&!` is not that unreadable, but once we reach stuff like `immut qux_t(y)* immut?!` is where we have problems. We might avoid this _operator soup_ by making stuff like `?` not a qualifier and instead it be denoted as `option(immut qux_t(y)*)`, or `qux_t(y) immut* option`, with qualifiers just being again unresolvable functions on types applied in a concatenative manner.\
this means that `qux_t immut option` is just `option(immut(qux_t))`. functions on qualifiers can be applied without any `~` operator explicited, and they are treated like other qualifiers are. this would allow for _true user-defined qualifiers_.\
using this syntax,

maybe I should cut all those typechecking ops, like `::`, `!:`, `?:`; these denote fundamentally different things, however.\
`::` is a type equality assertion that errors out if the lefthand side isn't of the same type as the righthand side.\
`!:` is a type equality assertion that errors out if the lefthand side isn't of a different type as the righthand side.\
`?:` returns true if the lefthand side is of the same type as the righthand side and false otherwise.\
clearly these cannot be represented by a single operator, except perhaps by using the special value `@error`, which I'd like to avoid.

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