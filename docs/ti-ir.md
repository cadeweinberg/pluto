# TI-IR: Target Independent Intermediate Representation

The TI-IR is the internal representation of the source code.
We Serialize the source, constructing the TI-IR. Then the
TI-IR becomes the data-structure that all subsequent portions
of the compiler operate upon.

From this observation I hypothesize that:

**Hypothesis 1:**

The form of the TI-IR has a great impact on the performance
of the compiler.

---

Consider two TI-IRs, A and B.
If we say that Form A takes up more memory, and is slower to access than form B.
Then we can say that a Compiler with TI-IR Form A will consume more memory and
will consume more time than a Compiler with TI-IR Form B.

## TI-IR: How can it be Structured?

To my knowledge there are two main forms of Intermediate Representation,
and many variations on those two forms.

1. Abstract Syntax Trees
2. Bytecode

### TI-IR: The Abstract Syntax Tree

An abstract syntax tree is a form of intermediate representation
which is based around
