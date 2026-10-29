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
C type declarations are _very ugly_; I really hate them. they're a pain to handle.

## How are types handled?
Normal type declarations can be handled exactly as variable declarations are.\
`typedef`s are trivial to handle as they only introduce _type aliases_ (not proper incompatible types), so we need only add an alias to whatever context we're in. Only issue with them is that they follow _C type declaration order_, which is hard to parse.\
What then?\
TODO: implement typedefs, proper C type order parsing, commaful C type declarations.