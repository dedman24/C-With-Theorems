### Changelog 2026/10/28.
 - expressions like `1+2;` are not parsed as `2+2;` anymore.
 - `cth` by itself now implies `repl`, like `python3` by itself does.
 - fixed rust-like integer suffixes not working.

couldn't work as much this time ;-; I want to add type support next!!!\
I want to finish cth. it's the project I love the most.

### Current errors:
 - all strings lead to segfaults.

### Things to add:
 - proper type support, types handled as variables.
 - working include stuff.
 - more ops (arithmetic & non).
 - '&&', '||', '^^' short-circuiting & not executing other stuff when it gets evaluated (within arbitrary expressions).
 - first-class support for arrays.
