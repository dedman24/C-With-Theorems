# Equality within theorems.
In the specification of some theorem, if its form is either:
> `theorem x(...) -> a == b;`\
> `theorem x(...) -> a != b;`

Equality is established when both have the same type and equal contents.\
Inequality is when either:
 * both have the same type and differing contents;
 * types differ, and cth states that different types by themselves cannot be equal.

If types differ, this would normally cause a typing error; it is however convenient to express inequality in this manner inside the conclusion of theorems, so we allow it there and nowhere else. See `ll.cth` for an example of where this helps, in `ll_no_loops`.\
It is also cumbersome to specify inequality both at the type & at the value level.