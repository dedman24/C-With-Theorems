# Is compatibility with C _that_ important?
To ake the c->cth transition as easy as possible, some may wish for cth to be 100% compatible with C code that respects some common guidelines (like not relying on undefined behaviour). A project would slowly start off as 100% C code, then slowy adopt more cth features (cth type handling, theorems & so on). No _critical mass_ is reached where the project has to be rewritten in cth, C code can stay as-is and enjoy full native-level interop with cth.\
One cannot rewrite C code in cth, because cth _is_ C. What changes is the (albeit tiny) boilerplate C code surrounds itself with.
At most, the largest departure from C would be cth's _universal function call notation operator_, the binary tilde (`~`).

This would make for a fine programming language, but projects like Rust and Zig work _despite_ not being _that compatible with C_.\
What gives?\
Compared to such languages, cth is heavily disadvantaged:
 - it has a small developer base (me rn basically);
 - it doesn't have that much support, institutional & non;
 - the '_theorems_' part of C With Theorems hasn't been formally fleshed out.

There are clear benefits to using it, but asking developers to migrate entire codebases to it just _is just not reasonable_, especially when the only short-term benefits are _cool function notation_, _better includes_ and `defer`. Theorems don't really come into play for large codebases as they hamper development by imposing a level of correctness very little production-ready software requires (not that theorems are useless, otherwise I wouldn't have included them as a language feature :PPP).

Theorems are useful in cryptography and in libraries where _security matters_, like in _network applications_.\
They are useless everywhere else, and most software is _everywhere else_. Again, it is unreasonable to expect _all code to be formally verified_; for big applications, only a small set of core functions _that rarely get changed_ can be; doing everything else would be too expensive.

## Conclusion.
**YES**, compatibility with C is important because:
 - C code can be migrated at no cost to cth;
 - sections of code can stay unmodified while other parts are rewritten in cth;
 - this allows for more projects to adopt & benefit from cth's native features;
 - cth code can be mixed with native C code freely, idiomatic ways from both languages can be used.

C With Theorems can still exist as a language _without_, just being some _imperative dependently-typed language_; I'm not saying it's a bad idea, but I want my language to be as similar to C as possible. Code your own langauge & btfo.