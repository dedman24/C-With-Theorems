### Changelog 2026/11/05.
 - fixed wording in README.md.
 - fixed issue with how qualifier is handled, in that _it didn't work_ and _it broke C multiword types_.
 - fixed memory leaks.
 - added proper error reporting for types.
 - added proper type compatibility.
 - added `const`, `immut` detection in assignment.
 - added proper errors when a variable is used _undefined_ or _unassigned_.
 - something like `1 = 2;` used to assign the value `2` to the literal `1`; now errors out instead. 

> `BEGIN snca;`

cth is shaping up to be something neat. I quite like it.\
LORD help me. I'm still not coding as much as I would've liked to.

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