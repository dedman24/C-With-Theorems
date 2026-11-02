# Type handling in cth.
Types in C are a pain. Something like `a * b` can be either:
 - the product of variables `a` and `b`,
 - the declaration of a variable `b` of type `a*`.

This means the compiler has to handle type declarations when parsing stuff.

Two different type declaration methods exist:
 - C-style `typedef`s, which are _discouraged when possible_ and only slightly better than using `#define` for types;
 - since we want types as variables, types are declared using the `type` type just like variables are.

The former are handled through special syntax rules, while the latter is handled like variables are.\
In general, cth can figure out the types of variables in declarations _when forming the ast_.

## C type declarations.
C type declarations are _very ugly_; I really hate them. they're a pain to handle.\
C types that span multiple tokens, like `long int`, `long long int` & so on are handled through _type joining_, where types of different names are joined.

## How are types handled?
Normal type declarations can be handled exactly as variable declarations are.\
`typedef`s are trivial to handle as they only introduce _type aliases_ (not proper incompatible types), so we need only add an alias to whatever context we're in. Only issue with them is that they follow _C type declaration order_, which is hard to parse.\

### Qualifiers as functions.
Qualifiers are to be handled as functions of types (`type qualifier(type);`) at the type level.

Certain qualifiers are _unresolvable_ (through the `@unresolvable` token), which means that they have no _definition_ and just exist by themselves.\
For example, if we model pointers as a function `type ptr(type);`, then it wouldn't make sense for `ptr(char)` to _evaluate to anything other than itself_. This is true for arrays too, which are just `type arr(type, size_t) = @unresolvable;`.
This also makes it evident for why `const char*` is not equal to `char*`; because `ptr(const char)` and `ptr(char)` are clearly different things.\
It also allows users to define _their own qualifiers_, that _explicit the function syntax_.

Qualifiers that _do not introduce type-level incompatibilities_, like `immut` and `const`, are handled how?
These are functions too; _implicit conversions_ exist between types of the form `immut(x)` and `x` which _make them into lvalues_ (so that assignment is invalid).

Explicit type functions are accessible through `stdc.qualif.ptr`, `stdc.qualif.arr` & so on. `const char *` can be also written as `(const char)~ptr()` or `char~const()~ptr()` (thanks to UFCS).

At the compiler level, types are _represented through abstract syntax trees_.
Something like `const char*` is effectively a linked list, but _arrays_ and _structs_ benefit from this model.\
The following happens:
 - types hold an abstract syntax tree in their `internal` field, representing their type.
 - support for complex, multi-token C types is possible trivially.
 - qualifiers create a new type token
 - compatibilities between types exist on the basis of _contents_; if the contents of 2 types are _not exactly the same_ they are incompatible.\
   like values, type names are created ad-hoc & stored in scope specifiers.

## On enums.
These are handled by defining relevant constants.
We support tagged unions through the `enum` keyword uhh uhh idk how but someway trust :PPPP.

TODO: decide how enums work.

## On unions & structs.
We want to support these fully.\
Custom types are 99% of the time just structs or unions with special