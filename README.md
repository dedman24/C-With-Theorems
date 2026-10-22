# Lisp With Theorems.
'Lisp With Theorems', also known as lth, is a simple dependent-typed programming language with syntax inspired from LISP. It is meant to be the simplest possible dependent typed programming language one can reasonably implement.

## Building.
To build lth,
one just has to execute `./build/buildall.sh lth`, which handles everything. I didn't use traditional build systems because I don't know how to use them (_cba to learn lol_) & a shell script works just fine.

## Usage.
To compile a program, one writes `lth [options] file.lth`. All output is written to stdout, save for errors which are written to stderr.

The following mutually exclusive options can be used:
* `help` ~ prints help string to stdout.
* `version` ~ prints current version & copyright info to stdout.
* `check` ~ type-checks code.
* `repl` ~ executes command-line REPL, useful for debugging programs.
If no options are explicited, all code is type-checked & interpreted.

## Syntax.
Syntax is very simple, made to be visually unique & easy to read.\
**Functions** are written as `(deffun (fun-name arg type) return-type)`. If multiple return types are specified, they are written as a list.\
**Theorems** are written similarly, as `(defthe (the-name arg type) result)`. They can be thought of as functions taking some arguments & returning an object representing what the theorem proves.\
I tried to give lth similar expressiveness as other dependent-typed theorem provers like Coq or Lean. I consider it successful in this regard, although more work is needed to ensure it can prove _all kinds of theorems_.

lth is **functional**; for example, variables cannot be modified once assigned.\
This was done to make proof generation simpler, it helps one reason about the code & it fits neatly with the logic model chosen.

## Why LISP.
Personally I don't understand why people like LISPs so much. I find them very ugly _purely from a syntactic point-of-view_.
They however make for very simple interpreted languages, which is why I chose them to implement my first.\
Maybe I'll learn to love them, perhaps when lth has a compiler of itself & a thousand device drivers written in it.