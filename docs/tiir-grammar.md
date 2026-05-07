# tiir grammar

## Plan

1. Phase 1 - Scope and invariants: lock v1 inclusions (module/version, tags, symbols, function signatures and bodies, core instructions), lock v1 exclusions (full DWARF schema, target-specific syntax), and formalize delimiter/comment/identifier rules for deterministic lexing.
2. Phase 2 - C99 coverage matrix: enumerate first-wave C99 features (scalars, pointers, arrays, structs/unions, string literals, calls, returns, branching, globals), map each to canonical TIIR snippets, and explicitly mark postponed features.
3. Phase 3 - Complete EBNF: finalize unambiguous productions for module layout, tags, types, literals, symbols, functions, blocks, instructions, and operands; add lexical grammar (tokens, escapes, keywords, punctuators) and parser error-recovery synchronization points.
4. Phase 4 - In-memory model spec: define required node families (module, symbol, type, function, block, instruction, tag, operand), ownership/interning boundaries, ID/reference strategy, and structural invariants.
5. Phase 5 - Parser implementation handoff design: describe lexer state model, recursive-descent parser layout, diagnostics, and post-parse validation pass responsibilities.
6. Phase 6 - Verification and acceptance: require example coverage traceability, negative grammar cases, parse-walk dry runs, and a ready-to-implement checklist with no incomplete productions.

## Verification

1. Grammar consistency pass: no incomplete or ambiguous productions in tiir-grammar.md.
2. Feature traceability: every C99 feature row maps to explicit TIIR productions and in-memory entities.
3. Parser dry-run review: token and parse walkthrough for at least global symbol, function call, and branch-loop snippets.
4. API feasibility check: identify what can reuse current context/interner APIs versus what new TIIR node APIs are required.
5. Standards-fit check: ensure metadata and type/linkage concepts remain compatible with C99 plus ELF/DWARF/ABI intent at abstract IR level.

## Locked Decisions from Alignment

- v1 scope: core executable IR (not declarations-only).
- Pointer syntax: opaque ptr keyword.
- Metadata attachment: prefix tag-use blocks.
- Symbol identity: both named and numeric symbols.
- Debug metadata: generic tag schema for v1, not DWARF-specific node schema.

## v1 Scope (Locked)

This section is normative for TIIR v1. The lexer, parser, and in-memory model should be designed to cover exactly this scope.

### In Scope

- Module-level metadata
  - version tag syntax (`[!version: 1.0]`)
  - module metadata tags using generic key/value pairs
- Tag grammar
  - tag definitions (`tag !id = [ ... ]`)
  - prefix tag usage (`[!id]`) attached to declarations and instructions
  - list-form key/value tags (`[!key: value {, !key: value}]`)
  - generic debug metadata keys (for example: `!file`, `!line`, `!column`), without DWARF node schema requirements
- Symbol grammar
  - symbol identifiers in both forms: named (`@main`) and numeric (`@0`)
  - global symbol declarations/definitions for constants, variables, and extern functions
  - function signatures and function bodies
- Core executable IR grammar
  - labels/basic blocks
  - instruction syntax needed for first-pass C99 lowering examples (call, return, branch, load/store, basic arithmetic/compare/cast forms)
- Type/literal coverage required for first-wave C99 examples
  - scalar integers/floats/bool, strings, arrays, struct/union literals
  - opaque pointer type keyword (`ptr`)
- Lexical features
  - comments
  - deterministic tokenization rules for identifiers, punctuation, literals, and escapes

### Out of Scope

- Full DWARF schema encoding
  - no required DIE tree, no mandatory DWARF tag/attribute graph representation in TIIR v1 grammar
- Target-specific syntax
  - no backend-specific text forms for ELF relocation records, x86-64 machine ops, or ABI-specific assembly details
- Advanced completeness goals outside parser bootstrap
  - no requirement for full optimization semantics in v1
  - no requirement for complete C99 edge-case coverage in v1

### v1 Acceptance Criteria

- The grammar is complete enough to parse and represent the Hello World-style module shown in this document.
- Every in-scope feature above has at least one canonical TIIR example in this file.
- No EBNF production is left incomplete for in-scope constructs.
- The language grammar must remain LL(1) to keep parser implementation simple, readable, and fast.
- The parser can build a valid in-memory representation for symbols, tags, types, function blocks, and core instructions in this scope.

## Module Level Metadata

Preliminary v1 listing of required module-level metadata.

Module metadata may be written as either:

- single-entry tag lines: `[!key: value]`
- list-form tag lines: `[!key: value, !key: value, ...]`

Both forms are equivalent in v1.

### MLM: Required in v1

- `version`
  - TIIR grammar/IR version for parser compatibility.
  - Example: `[!version: 1.0]`
- `module`
  - Stable module (translation unit) identifier.
  - Example: `[!module: "hello_world"]`
- `source_language`
  - Frontend language family.
  - Example: `[!source_language: "C"]`
- `source_standard`
  - Language standard level used for semantic lowering.
  - Example: `[!source_standard: "C99"]`
- `source_file`
  - Primary input file path/name for diagnostics and debug metadata roots.
  - Example: `[!source_file: "source.c"]`
- `producer`
  - Compiler/frontend identity used for traceability.
  - Example: `[!producer: "charon 0.1.0"]`
- `target_triple`
  - Combined lowering target profile.
  - Format: `arch-abi-objfmt`
  - Example: `[!target_triple: "x86_64-sysV-elf"]`

### MLM: Required when object emission is requested

- `object_kind`
  - Declares intended output class.
  - Example values: `object`, `executable`, `shared`, `staticlib`
- `visibility_default`
  - Default symbol visibility policy for symbols without explicit metadata.
  - Example values: `default`, `hidden`, `protected`, `internal`

### MLM: Optional in v1

- `build_id`
- `source_hash`
- `timestamp_utc`
- `optimization_level`
- `debug_enabled`
- `target_asm` (derived convenience mirror of `target_triple`)
- `target_abi` (derived convenience mirror of `target_triple`)
- `target_object` (derived convenience mirror of `target_triple`)

### MLM: Canonical Module Header Example

```tiir
[!version: 1.0]
[!module: "hello_world"]
[!source_language: "C"]
[!source_standard: "C99"]
[!source_file: "hello_world.c"]
[!producer: "charon 0.1.0"]
[!target_triple: "x86_64-sysV-elf"]
```

### MLM: Equivalent List-Form Example

```tiir
[
 !version: 1.0, 
 !module: "hello_world", 
 !source_language: "C", 
 !source_standard: "C99", 
 !source_file: "hello_world.c", 
 !producer: "charon 0.1.0", 
 !target_triple: "x86_64-sysV-elf"
]
```

### MLM: v1 Validation Rules

- Exactly one value must be present for each required v1 key.
- Required keys must appear before symbol/function declarations.
- Duplicate required keys are a parse/validation error.
- Metadata key/value entries must use `!id` keys (for example: `!line: 10`).
- Unknown keys are allowed (forward-compatible), unless marked `required` by a future profile.

### MLM: Preliminary Grammar Spec

```ebnf
tag_def   = "tag" tag_id "=" "[" tag_list "]" ;
tag_use   = "[" tag_list "]" ;
tag_list  = tag_elem {"," tag_elem} ;
tag_elem  = tag_id [":" literal] ;
tag_id    = "!" id ;
```

## C99 Coverage

Coverage matrix authoring format using sections and subsections (no tables).

### Matrix Entry Template

#### X.Y Feature

#### X.Y Representative C Snippet

#### X.Y TIIR Canonical Form

#### X.Y Grammar Productions Required

#### X.Y In-Memory Nodes Required

#### X.Y Semantic Validation Rules

#### X.Y Lowering Notes (Target Independent)

#### X.Y Test Coverage Status

### 1. Translation Unit and Linkage Model

#### 1.1 Source File as Translation Unit

##### 1.1 Feature

A single C source file (after preprocessing) is treated as one translation unit. TIIR should represent this as one module with module-level metadata and an ordered declaration/definition stream.

##### 1.1 Representative C Snippet

```c
/* hello.c */
extern int puts(const char *);

int main(void) {
  return puts("hello");
}
```

##### 1.1 TIIR Canonical Form

```tiir
[!version: 1.0]
[!module: "hello"]
[!source_language: "C"]
[!source_standard: "C99"]
[!source_file: "hello.c"]
[!target_triple: "x86_64-sysV-elf"]

symbol @puts : (ptr) -> i32;
symbol @main : () -> i32:
  %0 = call (ptr) -> i32 @puts, ("hello")
  ret %0
```

##### 1.1 Grammar Productions Required

- module header/tag-use productions in the MLM section
- top-level declaration list (symbol declarations and function definitions)
- ordering-preserving module parse rule

##### 1.1 In-Memory Nodes Required

- module node
- module metadata collection keyed by `!id`
- top-level symbol table (externs + definitions)
- ordered top-level declaration/definition list

##### 1.1 Semantic Validation Rules

- exactly one effective module header per translation unit
- required module metadata keys must exist before first top-level symbol/function
- multiple definitions of the same non-inline object/function in one translation unit are invalid
- unresolved extern declarations are allowed at translation-unit scope

##### 1.1 Lowering Notes (Target Independent)

Treat translation unit boundaries as compilation boundaries for symbol visibility and diagnostics.
Preserve unresolved extern symbols for later link-time resolution.

##### 1.1 Test Coverage Status

Planned

- positive: one source file with extern declaration and one function definition
- negative: missing required module metadata
- negative: duplicate non-inline definition in the same translation unit

#### 1.2 Declarations vs Definitions

##### 1.2 Feature

In C99, a declaration introduces a name and type; a definition additionally allocates storage for an object or provides a function body. Storage class (extern, static, auto) and type qualifiers (const, volatile, restrict) are encoded via metadata tags. For object declarations, treat presence of an initializer as a practical definition signal. Multiple compatible declarations are allowed in one translation unit, but only one non-tentative definition per object/function is allowed. File-scope object declarations without explicit storage class or initializer are tentative definitions.

##### 1.2 Representative C Snippet

```c
extern int g;
extern int g;
int g = 42;

int f(void);
int f(void) {
  return g;
}
```

##### 1.2 TIIR Canonical Form

```tiir
[!linkage: external]
symbol @g : i32;

[!linkage: external]
symbol @g : i32;

symbol @g : i32 = 42;

symbol @f : () -> i32;

symbol @f : () -> i32:
  ret @g
```

##### 1.2 Grammar Productions Required

- symbol declaration form without initializer/body
- symbol definition form with initializer (objects) or body (functions)
- repeated top-level declarations for the same symbol name
- metadata tag productions for storage class (!linkage: external|internal|none) and qualifiers (!qualifiers, !volatile)

##### 1.2 In-Memory Nodes Required

- symbol entry with declaration/definition state
- optional initializer payload for objects
- optional body payload for functions
- redeclaration chain (or list of declaration sites) per symbol
- metadata tag storage for storage class and qualifiers per symbol

##### 1.2 Semantic Validation Rules

- compatible repeated declarations are allowed (same type, compatible storage-class tags)
- incompatible redeclarations are invalid
- at most one non-tentative definition per symbol in a translation unit
- object declaration with initializer is a definition
- object declaration without initializer and without extern tag is a tentative definition
- object declaration with extern tag and no initializer is a declaration only
- function declaration without body is not a definition
- function declaration with body is a definition
- tentative definitions not overridden by full definition are implicitly zero-initialized at end of translation unit

##### 1.2 Lowering Notes (Target Independent)

Emit at most one storage-producing object definition per symbol in module output. Preserve declaration-only symbols as unresolved extern-compatible entries for later link resolution.

##### 1.2 Test Coverage Status

Planned

- positive: multiple compatible declarations plus one definition
- negative: multiple object definitions in one translation unit
- negative: incompatible redeclaration type mismatch
- positive: function prototype followed by one function body

#### 1.3 External, Internal, and No Linkage

#### 1.4 Storage Duration: Static, Automatic, Allocated

#### 1.5 Scope: File, Block, Prototype

#### 1.6 Identifier Namespaces

### 2. Preprocessing and Lexical Surface

#### 2.1 Trigraphs and Digraphs

#### 2.2 Universal Character Names

#### 2.3 Line Splicing

#### 2.4 Comments (Block and Line)

#### 2.5 Preprocessing Tokens and Token Boundaries

#### 2.6 Macro Expansion Conformance Requirements

#### 2.7 Conditional Inclusion

#### 2.8 Include Model and Header Search Behavior

### 3. Type System Core

#### 3.1 Void

#### 3.2 Character Types

#### 3.3 Integer Types

#### 3.4 Boolean Type

#### 3.5 Enumerated Types

#### 3.6 Floating Types

#### 3.7 Complex Types

#### 3.8 Qualified Types: const, volatile, restrict

#### 3.9 Derived Types: Pointers, Arrays, Functions

#### 3.10 Incomplete vs Completed Types

#### 3.11 Type Compatibility and Composite Type Rules

### 4. Declarations and Declarators

#### 4.1 Declaration Specifiers

#### 4.2 Init Declarator Lists

#### 4.3 Pointer Declarators

#### 4.4 Array Declarators (Including Parameter Qualifiers)

#### 4.5 Function Declarators and Prototypes

#### 4.6 Abstract Declarators and Typedef Names

#### 4.7 Struct and Union Declarations

#### 4.8 Enum Declarations

#### 4.9 Bit-Fields

#### 4.10 Flexible Array Members

### 5. Initialization

#### 5.1 Scalar Initialization

#### 5.2 Aggregate Initialization

#### 5.3 Designated Initializers

#### 5.4 Nested Designators

#### 5.5 Zero Initialization Defaults

#### 5.6 String Literal Initialization for Char Arrays

#### 5.7 Compound Literals

### 6. Expressions and Conversions

#### 6.1 Primary Expressions

#### 6.2 Postfix Operators

#### 6.3 Unary Operators

#### 6.4 Cast Expressions

#### 6.5 Multiplicative, Additive, and Shift Expressions

#### 6.6 Relational and Equality Expressions

#### 6.7 Bitwise and Logical Expressions

#### 6.8 Conditional Operator

#### 6.9 Assignment and Compound Assignment

#### 6.10 Comma Operator

#### 6.11 Lvalue/Rvalue and Modifiable Lvalue Rules

#### 6.12 Integer Promotions and Usual Arithmetic Conversions

#### 6.13 Pointer Conversions and Null Pointer Constants

### 7. Statements and Control Flow

#### 7.1 Labeled Statements

#### 7.2 Compound Statements

#### 7.3 Expression and Null Statements

#### 7.4 Selection Statements: if/else, switch

#### 7.5 Iteration Statements: while, do-while, for

#### 7.6 Jump Statements: goto, continue, break, return

#### 7.7 Switch case/default Semantics and Fallthrough

### 8. Functions and Calls

#### 8.1 Function Definition Structure

#### 8.2 Old-Style vs Prototype Declarations

#### 8.3 Parameter Passing and Default Promotions

#### 8.4 Variadic Functions and va_list Interoperability

#### 8.5 Return Value Semantics (Including Struct/Union Return)

#### 8.6 Recursion and Reentrancy Assumptions

### 9. Objects, Memory, and Layout Semantics

#### 9.1 Object Representation and Alignment

#### 9.2 Padding and Trap Representations

#### 9.3 Strict Aliasing Constraints

#### 9.4 Effective Type Rules

#### 9.5 Sequence Points and Side-Effect Ordering

#### 9.6 Volatile Access Semantics

### 10. Diagnostics and Constraints

#### 10.1 Required Diagnostics for Constraint Violations

#### 10.2 Undefined, Unspecified, and Implementation-Defined Behavior

#### 10.3 Translation Limits Affecting Frontend and IR Generation

### 11. Runtime and Library Boundary

#### 11.1 Hosted vs Freestanding Assumptions

#### 11.2 Startup and Entry-Point Model

#### 11.3 Builtin/Runtime Hooks Needed by Lowering

#### 11.4 C Library Declarations as Extern Symbols in TIIR

## EBNF

```ebnf

```

## Questions

what is the desire to design a new target independent IR, as opposed to just
copying LLVM's existing target independent IR?

1. it makes the project scope smaller as we get off the ground
2. LLVM already exists, so this puts us in a direct "competition" with them.
    and while I am definitely inspired by the design of LLVM, I am not
    going into this with the intent to replace them, or supplant them,
    or one-up them, I want to design a code-generation system for my own
    compiler, and I want to have it be translatable into multiple target
    languages, it is only natural to define a target independent IR to
    accomplish this goal.
3. even though "charon" is going to be a c99 compliant compiler, using this
    framework, this has little reference to LLVM, or clang, simply because
    there are so many c compilers. tcc is one that comes to mind, which is
    respected for its tiny size. I hope charon is respected as a fast and small
    compiler, which produces decently fast and small code.
