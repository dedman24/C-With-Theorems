### Changelog 2026/11/20::2.
 - added support for stuff like `typedef i32 x, *y;`.
 - fixed memleak with qualified variables.
 - fixed error message about type joining being duplicated sometimes.

### pending changes.
 - arrays.
 - stricter type compatibility.
 - reusing old types in `cth_tokeniser_op__declaration_master_type`.
 - certain compound qualifiers (like `immut` & `const`) should be incompatible.

> `BEGIN snca;`

ughhh I wanna work harder but I also don't wannaaaa.\
gta6 comes out today. top10 slop projects anyone who plays it LOVE YOURSELF!\
jeez imagine lucia naked :furryfeet2:.

my plan is to add a little each day, so that I'll have a full programming language in a couple weeks' time or so.

cth is so small rn 85KiB when compiled using `./build/buildall.sh cth release`.\
I know it will bloat in size as soon as I add all operators, but I quite like how small it is.\
You could run this on an IBM PC I bet. I want to shrink its memory usage more though, I feel as if it's quite inefficient currently.

> `END snca;`

### Current errors:
 - all strings & characters lead to errors.
 - path stuff in repl doesn't work, for most errors no path is printed.
 - hex, octal, binary values do not work.
 - no leading 0 making values octal.

### Things to add (in no particular order):
 - full typechecking.
 - formalised typechecker.
 - `void`, `void*` types.
 - structs, unions, enums.
 - function types, function calls.
 - working include stuff with `include` operator.
 - more ops (arithmetic & non).
 - looping constructs.
 - operator overrides.
 - subtyping support fully replacing current type compatibility handling.
 - explicit type conversions added in AST at typechecking.