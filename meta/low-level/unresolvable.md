# The `@unresolvable` keyword.
`@unresolvable` specifies that a function _cannot be resolved_; the result of an unresolvable function is an object of the return type of the function defined as being the function invocation itself.

For example, say one has `int foo(int x) = @unresolvable; 3*foo(100);`. The result of the latter expression is the expression itself; it cannot be _further resolved_.\
This is helpful with _types_, but I can imagine it being a major source of bugs in code. All `@unresolvable` functions, therefore, must only exist at compile-time and no code actually executed at runtime may ever have a result containing an unresolvable function.