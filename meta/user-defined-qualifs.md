# True user-defined qualifiers in cth.
We can allow true user-defined qualifiers in cth while also making qualifier handling more standardised.
This is by considering the following:
> `type_t qualif`

equal to:
> `type_t~qualif()`

and something like:
> `type_t qualif(args)`

equal to:
> `type_t~qualif(args)`

