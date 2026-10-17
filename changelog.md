### Changelog 2026/10/17.
 - updated README.md.
 - added single & double quoted literals as variables (supports C-like escape sequences).
 - added basic interpreter. it can execute little so far, but I'm going to work on it these next few days.
 - added command-line REPL with 'satisfiable repl', which takes program input from stdin & executes it. Supports piping, like with python's REPL.
bugs may be present, proper bugfixing will take place tmrw.

### Current errors:
 - tokenise cannot find file.
 - repl segfaults on tokenisation.

### Things to add:
 - more ops.
 - first-class support for arrays & other types, perhaps even user-generated types, to ensure safety.
 - more complex types, ultimately giving us dependent type support..