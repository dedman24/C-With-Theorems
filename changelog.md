### Changelog 2026/11/07.
 - `true`, `false` as immediates of type `bool` with repl output support.
 - `constexpr`, currently does nothing.
 - removed cth-style function declarations.
 - pointer, reference qualifiers & arithmetic.
 - `NULL` as keyword compatible with all other pointers.

> `BEGIN snca;`

I HAVE TO WORK HARDERFUCKKKKKKKK!!!!\
I vvill trvst me (ev&doe I never do).

it's pretty cool how something like `i32* const` is actually handled correctly rofl.\
I wish I knew how to properly structure the compiler as to not make huge errors later on. LORD help me.

> `END snca;`

### Current errors:
 - all strings & characters lead to errors.
 - path stuff in repl doesn't work, for most errors no path is printed.
 - hex, octal, binary values do not work.
 - no leading 0 making values octal.

### Things to add (in no particular order):
 - arrays.
 - support for multiple definitions, like `int x = 1, y = 2;` handled the same way it is in C, perhaps add as special case in interpreter.
 - better type printing (possibly recursive).
 - typedefs.
 - structs, unions, enums.
 - working include stuff with `include` operator.
 - operator overrides.
 - more ops (arithmetic & non).
 - '&&', '||', '^^' short-circuiting & not executing other stuff when it gets evaluated (within arbitrary expressions).
 - support for '++', '--' pre and post increment
 - looping constructs.