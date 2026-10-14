# The Satisfiable programming language.
Satisfiable is a simple dependent-typed programming language that gets compiled down to SAT instances. It is meant to aide hardware designers into producing verifiably correct logic circuits. It is also quite simple.

## Building.

To build Satisfiable,
one just has to execute `./build/buildall.sh satisfiable`, which handles everything. I didn't use traditional build systems because I don't know how to use them (_cba to learn lol_) & a shell script works just fine.

## Usage.
To compile a program, one writes `satisfiable [options] program_name.sat > output_file.cnf`. The compiler doesn't check what files end with so that `.sat` can be anything really. All output is written to stdout, save for errors which are written to stderr.

The following mutually exclusive options can be used:

* `help` ~ prints help string to stdout.
* `version` ~ prints current version & copyright info to stdout.
* `check` ~ type-checks the code without compiling to SAT formulae.

If no options are explicited, all code is type-checked & compiled down to instances of the satisfiability problem.

## Syntax.
Syntax is very simple, made to be visually unique & easy to read.\
**Functions** are written as `funname : fn(src0: t0, src1, t1 -> td)`.\
**Theorems** are written similarly, as `theoremname : theorem(src0: t0, src1: t1 -> resulting codition)`. They can be thought of as functions taking some arguments & returning an object representing what the theorem proves.\
I tried to give Satisfiable similar expressiveness as other dependent-typed theorem provers like Coq or Lean. I consider it successful in this regard, although more work is needed to ensure it can prove _all kinds of theorems_.

Satisfiable is **functional**; for example, variables cannot be modified once assigned.\
This was done to make proof generation simpler, it helps one reason about the code & it fits neatly with the logic model chosen.

## CPU instances & the Satisfiable model.
Satisfiable expects to be given a logical circuit that maps an initial binary string, representing the _initial circuit state_, to another binary string of the exact same size, which is the _final state the circuit ends up in_ after being given the initial one. We call this an _instance_ of the circuit, as it is the circuit itself in all quantifiable manners, just with state transitions explicited. Going from a traditional logic design to this representation & back is _trivial_ but _out of the scope of this document_.

Theorems can be built on top of the various primitives that compose the instance, and later _extended over the whole_. One does not care what something does anymore; most important is _what can be proved about it_.

## Why compiling to SAT matters.
Satisfiable compiles circuits down into instances of the _satisfiability_ problem (**not 3sat**).
The difference between SAT & 3SAT is that in the former clauses have unbounded size. Converting between the two is trivial, however.

Compiling to SAT matters because a lot of tooling exists around SAT solvers meant to prove the correctness of circuit design. While one may claim the Satisfiable specification (with all its theorems) alone is enough, one may wish to prove statements about circuits outside of the programming language; we allow for exactly this. It's also pretty neat imo.\
It is possible one may find proving statements on certain SAT circuits using dedicated solvers much easier than proving them in Satisfiable.

One can turn off compilation to SAT by using `satisfiable check ...`, which just performs type checking. It prints whether it was successful or not to stderr.
