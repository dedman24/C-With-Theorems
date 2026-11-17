# How safety is implemented in cth.
C With Theorems tries to be a safe programming language; C tries to be a practical programming language.\
The two wills clearly conflict; how does cth ensure safety?

## Separate compilation mode.
It would be nice to have cth be its own language, have it be incompatible with C code and have good C interoperability instead.\
This is not why I designed cth, however. 'C With Theorems' would stop being _C_, it would just be '_With Theorems_'.

## Safety enforced regardless.
This is a rather controversial approach.\
We enforce the _highest level of safety on all code_, whether it be C or cth code. This means that a lot of C code breaks, and that it has to be rewritten, which is not something negligible one can skip over.

Strict safety can be enforced on _all code_ using compiler flags, but by default it doesn't work like this.\
Think of it as having `-Wall -Werror` as compiler flags to catch many simple bugs early.

## Safety is not enforced, can be proven.
This is the approach cth will follow: all safety is _opt-in_, no one stops you from writing _unsafe code_, safety is a property _one must prove about a given program_.