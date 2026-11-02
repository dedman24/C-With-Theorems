### Changelog 2026/11/2.
 - added support for C types like `short`, `short int`, `int` and so on, through 'type joining'.\
   while I'd like for `signed` to be a qualifier like `const` is, for it to be handled in the same manner, it's a type in C and it has to be handled like other multi-word types.\
   stuff like `signed u32` evaluates to `i32`, which is cool, although _not that useful_.
 - reworked 'internal' field of variables.
 - removed `cth_type__strings` (bug-prone, tables are also generally bad). 
 - made `sorry` become `@sorry`, added `@unresolvable` for functions that cannot be resolved (useful for modelling qualifers). added `@` as valid regex for names.
 - added `cth_ast__funcallT`.
 - added `char16_t`, `char32_t`, `size_t`, `ssize_t` as default types. `unsigned ssize_t` becomes `size_t` and vice-versa.
 - started work on C qualifiers, currently only supports simple stuff like `const` and `immut`.\
   types are now trees, and qualifiers functions on types. this also allows for user-defined qualifiers.
 - types initialised as tree nodes; qualifiers don't really need this.

> `BEGIN snca;`

I want to finish cth. It's all I can thinka bovt, when I wake up or when I turn on my pc.\
It nags the back of my mind, which is crazy because I don't even work on it that hard; I am a lazy person at my core, I don't work as hard as I _could_.\
I must work harder. Time lost is never made up for.

> `END snca;`

### Current errors:
 - all strings & characters lead to errors.
 - path stuff in repl doesn't work, for most errors no path is printed.
 - hex, octal, binary values do not work.
 - no leading 0 making values octal.

### Things to add (in no particular order):
 - typedefs.
 - pointers.
 - arrays.
 - `true`, `false` as literals.
 - structs, unions, enums.
 - better differentiation between rvalues & lvalues using variable state.
 - working include stuff with `include` operator.
 - operator overrides.
 - more ops (arithmetic & non).
 - '&&', '||', '^^' short-circuiting & not executing other stuff when it gets evaluated (within arbitrary expressions).
 - support for '++', '--' pre and post increment
 - looping constructs.