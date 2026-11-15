### Changelog 2026/11/15.
 - something like `type my_i32 = i32 immut;` segfaulted, doesn't anymore.
 - compound qualifiers (like `static const`, `immut const`) don't segfault anymore.
 - added output of `_Qualifier_t`-typed variables, doesn't work for one-qualif case but doesn't matter tbh.

### pending changes.
 - arrays.
 - typedefs.
 - reusing old types in `cth_tokeniser_op__declaration_master_type`.
 - certain compound qualifiers (like `immut` & `const`) shoul be incompatible.

> `BEGIN snca;`

I really want to do more.\
I've been getting into language learning lately. I think I've got a knack for it.\
My approach to keeping oneself busy is to always have tons of different stuff to do, so that when one doesn't want to do x another activity can be done.\
For example, when I don't want to code, I can grind vocab on anki or consume content in the target language.\
The issue is that I've been treating cth more as a hassle than anything else.

LORD help me.

> `END snca;`

### Current errors:
 - all strings & characters lead to errors.
 - path stuff in repl doesn't work, for most errors no path is printed.
 - hex, octal, binary values do not work.
 - no leading 0 making values octal.

### Things to add (in no particular order):
 - full typechecking.
 - `void`, `void*` types.
 - structs, unions, enums.
 - function types, function calls.
 - working include stuff with `include` operator.
 - more ops (arithmetic & non).
 - support for `op=` operators (like `+=`) with them being handled as syntax sugar.
 - looping constructs.
 - operator overrides.
 - subtyping support fully replacing current type compatibility handling.