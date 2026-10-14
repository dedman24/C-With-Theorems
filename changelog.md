## Changelog 2026/10/something.
 - added build instructions to git.
 - fixed segfaulting when source file is missing.
 - fixed tokenisation segfaulting on names & values.
 - added build commands.
 - updated README.md.

I think I should have tokenisation output an AST _directly_, because I want to get to an interpreter ASAP.\
After ASTs, the interpreter would be the thing to add.