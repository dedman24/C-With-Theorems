### Changelog 2026/10/18.
 - fixed tokeniser not being able to find file (_room-temp iq error idk how that slipped_).
 - fixed segfaulting on repl.
finally one can execute `./satisfiable repl` and have it evaluate what 1 + 2 is TwT...

### Current errors:
 - values terminating in standard rust-like integer suffixes do not work properly, lead to segfaults.

### Things to add:
 - more ops.
 - first-class support for arrays & other types, perhaps even user-generated types, to ensure safety.
 - more complex types, ultimately giving us dependent type support.
 - path stuff to interpreter probably first thing I'm ading tmrw.