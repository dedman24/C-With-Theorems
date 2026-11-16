# When errors are a part of theorems.
One can specify a proof that a given condition of a theorem must error out, and therefore that it is invalid, as:
> `theorem x(...) EXPRESSION => @error;`

The error specified usually being a type error.