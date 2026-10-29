### Changelog 2026/10/29::2.
 - added `::` operator, which is explicit compile-time typechecking enforced through type errors.
 - fixed issues with type suffixes caused by old design.
 - added variable declarations (`int32_t x = 10;`), testable in repl _right now_ (run `echo "int32_t x = 2; x + 1;" | ./cth`).
 - printing a variable or a literal by itself (like `100;`) now prints its value. 
 - same variable/literal in two different statements in repl had different variableT context; not anymore :PPP.

> `BEGIN snca;`

fuckk I am so shit at coding why do I code so slowly I remember writing 400loc/day last week why can't I code as quickly anymore!!!!\
SOMEONE PLEASE HELP ME I FUCKING HATE THIS! I need to finish cth as quickly as possible.

> `END snca;`

### Current errors:
 - all strings & characters lead to errors.
 - path stuff in repl doesn't work, for 99% of errors no path is printed.

### Things to add (in no order in particular):
 - special type for internal formats of variables.
 - typedefs.
 - qualifiers (arrays especially).
 - working include stuff.
 - operator overrides.
 - more ops (arithmetic & non).
 - '&&', '||', '^^' short-circuiting & not executing other stuff when it gets evaluated (within arbitrary expressions).
 - support for '++', '--' pre and post increment
 - looping constructs.