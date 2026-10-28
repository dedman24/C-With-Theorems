# Issues with C.
If we want for us to be _source-compatible with C_ with a vast majority of the code available, we have to sacrifice _some safety_. This is what the proponents of many memory-safe languages say; it is therefore _impossible_ to make a programming language that is both:
 * almost entirely source compatible with C.
 * memory safe, with dependent typing and much more.

We wish to prove them wrong. Memory safety _IS_ possible in C.

This isn't meant as an attack on C; the language _isn't perfect_ but I _love it_ which is why cth is closely compatible with it.\
C With Theorems is meant to be _exactly that_: C with some extra stuff that _would never be added in the language otherwise_, but that I think would be good ideas to _breathe new life into it_ (not that it _needs_ new life lol I just think it would be cool).

## Type handling.
C is an _inherently unsafe language_, both memory-wise and type-wise. Implementing dependent typing with _implicit type conversion_ is hard.\
We can handle it through _subtypes_; conversion from a subtype to its master type is allowed _if not performing it would lead to type errors_.\
We can have char be a subtype of short, short a subtype of int, int of long long and so on. we also have our own _type-safe types_, like i32 and u64 and so on, which _enforce type safety_.

Regarding something like `typedef another_t some_t;`, we can have it be a more sophisticated `#define some_t another_t`. Types are meant to be defined like variables are, using `type some_t = another_t;`.\
Variables of type `type` are const. `some_t` when using cth type declarations is not compatible with `another_t`, but it would be when using `typedef`.

## Aliasing rules.
C's aliasing rules are ugly, but they exist for a reason. We propose to overhaul them.\
 1. No more strict aliasing for explicit casts; this means that if `x :: i32*`, then `(u16*)x` does not lead to errors, as `u/iX` types are defined by their binary representation.\
    This doesn't break any existing C program because what we define here is otherwise _undefined behaviour in C_; in general, _having undefined behaviour in C_ be _well-defined in cth_ does not break existing C code (by the definition of undefined behaviour), but it means said cth code is not compatible with the C specification (this is fine; C -> cth is what we're after, not cth -> C).
 2. `char*` can still alias all types; `char` still exists for compatibility with C, but for integer types `u/i8` is recommended and for strings either the special-purpose `string` type (`stdc.string`) or `char8_t` are recommended; _due to strict aliasing rules_, these cannot alias with any ptr.
 3. `alias` can now specify aliasing _with more granularity_: given a function `void foo(i8* a, i8* b, i8* c, i8* d)`, by default a, b, c and d may all alias; if one wanted to express that a may only alias b and c may only alias d, one could as:\
     `void foo(i8* a, i8* b, i8* c, i8* d) alias(a, b), alias(c, d);`\
     where `alias` specifies what aliases with other pointers. Without the alias specifies all of the variables would've aliased, this only _explicits which ones do_.\
     `alias` isn't a keyword, just some syntactic sugar that can be written after a function to explicit aliasing better. It can be defined as whatever by the program.\
     `restrict` by itself after a function states that _no pointers alias_. 
 4. `restrict` still exists and it acts as it does in C; it is still _very powerful_. 

## Pointer safety.
Normal C pointers are nullable, which in theory means that for maximum safety one should check if any function returning a ptr returns `NULL` or not. This is _cumbersome_ and rarely done in code.\
We keep the traditional nullable pointer `*` and add a non-nullable pointer `&`, reminiscent of C++ references. `&` pointers are still pointers, though, and they behave exactly like `*`, the only difference being that `*` can encode the error value `NULL` and `&` can't. One can think of `*` as similar to `&?` (optional reference), although the two are different.\
Code inside `.cth`, `.hth` files (when compiled with the highest level of safety) _must_ unwrap nullable pointers, now defined as `enum{ ...* some, NULL }`, with something akin to `switch ptr NULL = ..., some = ...;` or an _error is generated_. See option types if something isn't clear.

## Option types.
To further aide error handling, we introduce the `?` option type; at its simplest, a type `qux_t?` is an enum `enum{ qux_t some, none }` that cannot be used without _it being unwrapped_ (and therefore without one checking for errors).\
A more explicit error type `type error_t(type t, type err) = enum{ t some, err none }` can be used to report more complex errors. It is provided with `include stdc.error`.

## Mutability and const.
`const` in C is horrible, because code isn't actually forced to follow it. Given a `const char* x`, `*(char*)x = 10;` compiles, when it _shouldn't_.\
We introduce the `immut` qualifier, which is like `const` only that it cannot be _type-casted away_. Given `immut char* x`, `(char*)x` generates a type error since _immutable values must always stay immutable in the part of the program they're defined_.\
Much more powerful is `constexpr`, which acts like `consteval` in C++. This too cannot be removed from values.

## Ugly includes & boilerplare.
The usual way of including files in C (through `#include`) is very ugly because it has lots of special rules one must follow (end file in newline, have file guards, split header-implementation files, `#define WHATEVER_IMPLEMENTATION` before inclusion somewhere & forget). I also quite dislike how standard libraries are included; many stdc libs from C99 and before have names that often conflict with user headed files (`math.h` is too generic for it to be a stdlib's name).\
For these reasons cth has a secondary way of including files, exposed to programs ending in `.c/.h` as `_Include` and to `.cth/.hth` as `include`. It is an operator that, if assigned to a variable of type `namespace`, has that variable act as a de-facto namespace for its contents; otherwise, if it isn't assigned, the included file's contents are available without namespacing.\
Example usage:

> `namespace foo = include "foo.cth";`\
> `// the contents of 'foo.cth' are exposed under the variable foo, and accessible as normal struct elements.`\
> `// if foo.cth exposes two functions 'add(void)', 'mul(void)', then one may access them as:`\
> `foo.add(); foo.mul();`

`include` handles files being included multiple times in the same project _by itself_; therefore no include guards _have to be used_.

Regarding standard libraries, these aren't paths anymore; all standard libraries are automatically accessible through the `stdc` variable, which is just a collection of namespaces; instead of including `"stdlib.h"` to use `malloc`, one can just write `stdc.lib.malloc`, with `stdc.lib` being a namespace. `stdc` is a variable accessible in files ending with `.cth` or `.hth`; in `.c`, `.h` files it is accessible as _Stdc.\
Fear not program size bloat, for cth _should_ only pull into the code what it effectively uses from standard libraries.

Including a `namespace`-typed variable either:
 - is a complicated way to assign a namespace to another (if written like `namespace newlib = include stdc.lib`); or
 - includes said namespace for use thorought the program (if not assigned to any other variable).

`namespace`-typed variables are always immutable; the `namespace` type also cannot be passed to functions, and what function is being called through a namespace must be _determined at compile-time_.

Namespaces mean that function names don't devolve to `program_component_struct_operation`, which is really ugly over a large enough project; since cth supports _universal function call notation_, they at worst become `struct~program.component.operation();`, which tbf isn't really a ground-breaking improvement but at least the programmer has the _freedom_ to choose what's convenient & pretty at any given time.

## Types ending in '_t'.
POSIX reserves all types ending in `_t`, which is unfortunate because it is the best way of terminating types. cth namespacing means that user-defined types ending in `_t` need not necessarily conflict with POSIX ones, so all is saved.\
~~seriously screw the guys at POSIX why even reserve the `_t` prefix it is _ubiquitous_!!!~~

## Precedence.
This might be a dumb idea, but the rules of operator precedence in `.c`, `.h` files is somewhat different than the rules in `.cth`, `.hth`.\
The only difference is that bitwise operators have _higher precedence_ than relational ones in `.cth`, `.hth` (while in ISO C they don't).
The C way is legacy cruft from an evil age that doesn't really make sense. This split precedence stuff may be changed in the future, but if I had to change _one_ thing about C it would be this.\
Seriously, in any _modern_ language `if(1 & 0 == 0) printf("hello\n");` wouldn't do anything, but in C it _prints_ because it's treated as `if(1 & (1 == 0))`, which evaluates to `if(1 & 1)` TwT...

_This is subject to change_; that the same code _fundamentally behaves differently_ in C code compiled with cth vs cth code is bad. cth should be a _drop-in replacement for C_...

## Small standard library.
A large standard library isn't bad. A large standard library is only bad if the language standard doesn't offer default definitions of std library files.\
I want the cth stdlib to hold everything one might need; I want it to be the largest collection of CC0 code ever made.\
Some standard libraries should be `stdc.regex`, `stdc.http`, `stdc.html`, `stdc.crypto`; all should be reasonably performant, not architecture-dependent and _proven to be secure using the dependent typing facilities built into cth_.\
Using standard libraries should be very easy; one doesn't even need to `include` them, one can use them through the `stdc` namespace.

## Include hell.
Wanting to support the majority of existing C code often means that one is forced to relegate every interesting feature into its own separate standard library. C code ends up with a fat list of `#include` directives at the start; we wish not to do this, as it makes code very ugly and it increases the amount of boilerplate one ends up writing.\
By default, files ending in `.hth` include the following standard libraries:
  - `stdc.def` ~ everything normally provided by `stddef.h`.
  - `stdc.int` ~ `uX`, `iX` types corrisponding to their `[u]intX_t` counterpart, now safer and defined as their binary representation.
  - `stdc.bool` ~ boolean types.
  - `stdc.char` ~ `charX_t` data types.
  - `stdc.type` ~ `type` type, otherwise just `_Type`.
  - `stdc.defer` ~ `defer` blocks, otherwise just `_Defer`.
  - `stdc.error` ~ `error_t` type.
  - `stdc.immut` ~ `immut` qualifer, otherwise just `_Immut`.
  - `stdc.atomic` ~ all `stdatomic.h` and `_Atomic` now can be written as just `atomic`.
  - `stdc.string` ~ `string` type and more.
  - `stdc.include` ~ `include` operator, otherwise just `_Include`.
  - `stdc.namespace` ~ `namespace` type, otherwise just `_Namespace`.

This means C code can have defined what otherwise is an important keyword in cth, without breaking anything.
