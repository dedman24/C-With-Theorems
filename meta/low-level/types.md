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