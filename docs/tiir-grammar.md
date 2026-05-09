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
- Parenthesized operand-pack syntax `( ... )` is reserved for variadic instruction operands only.
- Aggregate indexing: `gep` accepts integer indices for all aggregates; for structural types with tagged members, `gep` may accept a member tag `%id` and resolve it in the struct-local namespace.
- Metadata attachment: prefix tag-use blocks.
- Metadata key syntax: all metadata tags use `!id` prefix (e.g., `!version`, `!storage`, `!linkage`).
- Symbol identity: both named and numeric symbols; global symbols use `@` prefix, local (block-scoped) symbols use `%` prefix.
- local symbols which are numeric are considered unnamed symbols, and their numbering begins from 0, all numbered locals within
  a given scope share the same pool of numbers, e.g. parameters, basic-blocks, virtual registers and local stack space.
- Debug metadata: generic tag schema for v1, not DWARF-specific node schema.
- Grammar LL(1) compliance: parser must be implementable as recursive-descent without lookahead conflict.
- instructions use a standard layout e.g. `add i32 %4, %a, 5`
- instructions are annotated with a type, this type is shared among all operands to the instruction. (except for particular instructions,
  such as load, which expects a pointer type to load from, etc.)
- instructions destination is always the leftmost operand, and it always introduces a new name to the local scope. (except for particular
  instructions such as 'ret' which only consume operands. e.g. `ret %3` etc.)
- instruction operands with variable arity are passed as a parenthesized operand pack.
- `call` passes arguments using parenthesized operand-pack syntax.
- automatic storage with block-bounded lifetime uses explicit `alloca` / `dealloca` pairs.

## v1 Scope (Locked)

This section is normative for TIIR v1. The lexer, parser, and in-memory model should be designed to cover exactly this scope.

### In Scope

- Module-level metadata
  - version tag syntax (`[!version: 1.0]`)
  - module metadata tags using generic key/value pairs
- Tag grammar
  - tag definitions (`tag !id = [ ... ]`)
  - prefix tag usage (`[!id]`) attached to declarations and instructions
  - inline tag usage (`... [!key: value {, !key: value}]`) on symbol/instruction statements
  - list-form key/value tags (`[!key: value {, !key: value}]`)
  - generic debug metadata keys (for example: `!file`, `!line`, `!column`), without DWARF node schema requirements
- Symbol grammar
  - global symbols use `@` prefix: named form (`@main`, `@g`, `@f`) or numeric form (`@0`, `@1`)
  - local (block-scoped) symbols use `%` prefix: named form (`%a`, `%p`) or numeric form (`%0`, `%1`, `%N`)
  - global symbol declarations/definitions for constants, variables, and extern functions
  - function signatures and function bodies with local symbol declarations
- Core executable IR grammar
  - labels/basic blocks
  - instruction syntax needed for first-pass C99 lowering examples (call, return, branch, load/store, alloca/dealloca, basic arithmetic/compare/cast forms)
  - parenthesized operand packs for variadic instruction operands (for example call arguments)
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

## TIIR Specification (Consolidated First Pass)

This subsection consolidates the current TIIR v1 grammar and semantic rules into
a C99-like layout. It is a first-pass normative index over rules already
defined later in this document, organized by construct family (scope/visibility,
types, instructions/lifetime, metadata).

### 1. Translation Unit, Scope, and Visibility

- One TIIR module corresponds to one translation unit with exactly one effective module header.
- Required module metadata keys must appear before first top-level symbol/function.
- Global symbols are `@id` and local symbols are `%id`.
- Linkage is modeled by `!linkage` with `external`, `internal`, `none`.
- Repeated compatible declarations are allowed; incompatible redeclarations are invalid.
- At most one non-tentative definition per symbol is permitted per translation unit.
- Block scope lifetime is explicit through `alloca` / `dealloca` pairs.
- Labels are function-local; branch targets must resolve in-function.
- Struct member tags use `%id` but resolve in struct-local member namespace, not block/function scope.

### 2. Type System

#### 2.1 Scalar Types

- Integers are explicit-width `iN`/`uN`; widths currently constrained to `1, 8, 16, 32, 64`.
- Boolean is `i1`.
- Floating-point types in v1 are `f32` and `f64`; `f16`/`f128` are extension-gated.
- Character-family C types lower to integer types; no distinct character primitive in TIIR.
- Enums lower to integer semantics and may be represented with `enum { @ELEM = value }` type descriptors.

#### 2.2 Pointers, Functions, and Operands

- Pointers use opaque `ptr`.
- Function type form is `(param_types) -> return_type`.
- Parenthesized operand packs `( ... )` are instruction-level variadic operand groups, not value literals or type forms.
- `call` consumes one operand pack for arguments; `ret` returns either no operand or one typed operand.

#### 2.3 Structural Types (Struct/Union/Enum)

- Named structural types are `type @Name = struct ...`, `union ...`, `enum ...`.
- Struct elements may have optional member tags: `struct { %tag: T, ... }`.
- `gep` selector may be integer index or `%memberTag` when base type is structural and member tag exists.
- Member tags must be unique within a structural type; cross-type reuse is allowed.

#### 2.4 Arrays, VLA, and Flexible Array Members

- Fixed array form is `[N T]` with positive compile-time `N`.
- Runtime-sized array form is `[* T]`.
- `[* T]` is valid for automatic-storage VLA declarations (function scope) and struct trailing flexible members.
- VLA lowering requires `alloca` plus matching scope-exit `dealloca`.
- Flexible array members are struct-only, trailing-only, and not the only named member.

#### 2.5 Qualifiers and Completeness

- Qualifiers are metadata-based via `!qualifiers` with atoms `const`, `volatile`, `restrict`.
- `restrict` is only valid for pointer-typed declarations.
- `void` is incomplete and cannot be object type, array element type, or structural member type.
- Named `struct`/`union` may be declared incomplete then completed once with compatible kind/layout.

### 3. Instructions and Lifetime Semantics

- Core instruction families include control flow, arithmetic/compare, memory access, call/ret, and stack lifetime (`alloca`, `dealloca`).
- Arithmetic instructions require type-consistent operands of the declared instruction type.
- Branch conditions must be `i1`; non-`i1` conditions require explicit compare.
- `alloca` / `dealloca` model block-bounded automatic storage extent.
- Heap lifetime is represented via allocation/free calls and storage/lifetime metadata.

### 4. Metadata and Annotation Model

- Metadata keys use `!id` syntax.
- Module-level metadata supports required, optional, and forward-compatible unknown keys.
- Declaration/instruction metadata may be prefix or inline; forms are equivalent in v1.
- Qualifiers, linkage, storage, and debug/source mapping are represented via metadata tags.

### 5. Validation Constraints (Complete Current List)

This is the canonical grouped validation checklist for the current TIIR v1
first pass.

Module and translation unit constraints:
- Exactly one effective module header per translation unit.
- Required module metadata keys must exist before first top-level symbol/function.
- Duplicate required module keys are invalid.
- Multiple definitions of same non-inline object/function in one translation unit are invalid.
- Unresolved extern declarations at translation-unit scope are allowed.

Declaration/linkage/storage constraints:
- Compatible repeated declarations are allowed.
- Incompatible redeclarations are invalid.
- At most one non-tentative definition per symbol per translation unit.
- Object declaration with initializer is a definition.
- Object declaration without initializer and without extern is tentative definition.
- Object declaration with extern and no initializer is declaration-only.
- Function declaration without body is declaration-only; with body is definition.
- Tentative object definitions not overridden are implicitly zero-initialized.
- External linkage redeclarations must agree on linkage.
- No-linkage symbols have no cross-translation-unit visibility.
- File-scope object defaults to `!storage: static`.
- Block-scope non-static object defaults to `!storage: automatic`.
- Block-scope `static` maps to `!storage: static`.
- Allocated-storage objects are created by allocation operations, not direct declarations.
- Allocation/free-style lifetime edges must be consistent (no use after free when detectable).
- Storage metadata on redeclarations must be compatible.
- Deallocation sites must reference prior allocations.
- Every `alloca` must have matching `dealloca` on normal scope-exit paths.

Scope and namespace constraints:
- File-scope names are visible from declaration point to translation-unit end.
- Local scope names are visible only in declaring block extent.
- Prototype parameters are scoped only within declaration.
- Ordinary identifier conflicts within same scope are invalid.
- Type-tag redefinition must be compatible; incompatible redefinition invalid.
- Struct/union member tags must be unique per type.
- `gep ... , %tag` requires structural base type and resolvable member tag.
- `%tag` selector resolution is in struct scope, not function/block scope.
- Basic-block labels must be unique per function.
- Branch target label must exist in same function.

Type constraints:
- `void` cannot be object type, array element type, or structural member type.
- Integer literals must be representable in declared integer type.
- Signed constant-expression overflow is invalid; unsigned wrap is defined.
- Mixed-width integer arithmetic requires explicit conversion before operation.
- `i1` literals are only `0` or `1`.
- Arithmetic operations on `i1` are invalid.
- `and`/`or`/`xor` on `i1` are valid.
- Branch condition must be `i1`.
- Bool conversion must be explicit compare-to-zero form.
- Enum initializers must be compile-time integer constants.
- Enum implicit values increment previous resolved value by `+1`.
- Duplicate enum element names are invalid.
- Enum values must fit selected underlying integer type.
- Enum identifiers are referenced as `@id` in operands.
- Enum name collisions across enums in same translation unit are invalid.
- In v1, only `f32` and `f64` are valid float types unless extension gate enabled.
- Floating arithmetic operands must have identical float width.
- Integer/float mixed operations require explicit conversion.
- Complex values must use homogeneous two-element structural representation.
- Dedicated complex opcodes are invalid in v1.
- Complex component selectors must be valid (`0`/`1` in current representation).
- Unknown qualifier atoms are invalid.
- Duplicate qualifier atoms in one qualifier set are invalid.
- `restrict` on non-pointer declarations is invalid.
- `const` forbids stores through the qualified declaration/reference.
- Qualifier redeclarations must be compatible.
- Array element type must be complete and non-`void`.
- Function parameter raw array/function forms must be decayed before TIIR emission.
- Direct call argument arity/types must match function signature.
- Fixed-size array length must be positive compile-time integer.
- `[* T]` is valid only in function-scope automatic storage or flexible-array-member context.
- `[* T]` is invalid for file-scope/global symbols.
- `[* T]` as struct member is valid only for trailing flexible member with at least one prior named member.
- Operand-pack element arity/types must match consumer instruction context.
- `ret` with no operand is valid only for void-returning functions.
- `ret` with operand is valid only for non-void-returning functions.
- Named structural types may transition incomplete -> complete at most once per module.
- Structural completion must preserve declared kind (`struct` vs `union`).
- Object declarations of incomplete named structural types are invalid until completion.
- Fixed-size array element type must be complete at declaration time.
- Flexible array members are struct-only, final-member-only, and not sole named member.
- Flexible array members carry no explicit constant bound.

### 6. Required Tests (Complete Current List)

This is the canonical grouped required test checklist for the current TIIR v1
first pass.

Translation unit / metadata tests:
- positive: source file with extern declaration and one function definition.
- negative: missing required module metadata.
- negative: duplicate non-inline definition in one translation unit.

Declaration / linkage / storage tests:
- positive: multiple compatible declarations plus one definition.
- positive: function prototype then one body.
- positive: external and internal symbols at file scope.
- positive: function with internal linkage.
- positive: static local variable.
- positive: extern declaration in function scope.
- positive: file-scope static object and block-scope automatic object.
- positive: block-scope static local.
- positive: heap allocation with deallocation before return.
- positive: allocation marked escaped.
- negative: multiple object definitions in one translation unit.
- negative: incompatible redeclaration type mismatch.
- negative: conflicting linkage redeclarations.
- negative: multiple external definitions across modules.
- negative: incompatible storage-class redeclaration.
- negative: use-after-free.
- negative: missing deallocation in leak-check mode.

Scope / namespace / control-flow tests:
- positive: global symbol visible from global and local scopes.
- positive: local symbol visible in local scope.
- positive: struct type definition with member access by index or tag.
- positive: shadowed local identifier behavior.
- positive: goto lowered to branch with valid label.
- negative: global symbol used before declaration.
- negative: local symbol used before declaration.
- negative: local symbol visible from global scope.
- negative: duplicate ordinary identifier in same scope.
- negative: duplicate member tag/name in one struct type.
- negative: branch to undefined label.
- negative: incompatible struct redefinition.

Type-system tests:
- positive: void-return function declaration/definition and bare `ret`.
- positive: pointer parameter for `void *` lowering.
- positive: `i8`/`u8` character-lowered declarations and stores.
- positive: string literal lowered to null-terminated array symbol.
- positive: integer width/signedness declarations and literals.
- positive: max/min representable integer literals.
- positive: bool conversion and bool branch.
- positive: enum implicit/explicit values and enum symbol use in compare/branch.
- positive: `f32` and `f64` arithmetic.
- positive: float comparison feeding branch.
- positive: complex add and multiply lowering; real/imag access by `gep`.
- positive: volatile global access behavior.
- positive: restrict-qualified pointer parameters.
- positive: valid call operand-pack matching callee signature.
- positive: local VLA lowered to `alloca` + scope-exit `dealloca`.
- positive: valid trailing flexible array member with prior named field(s).
- positive: forward struct declaration, compatible completion, then object declaration.
- negative: `ret` operand present in void function.
- negative: bare `ret` in non-void function.
- negative: object declared with type `void`.
- negative: integer literal out of range.
- negative: mixed-width integer arithmetic without explicit cast.
- negative: arithmetic on `i1`.
- negative: branch on non-`i1` without explicit compare.
- negative: enum identifier used without `@`.
- negative: duplicate enum element name.
- negative: non-constant enum initializer.
- negative: enum explicit value out of underlying-type range.
- negative: `f16` or `f128` use without extension gate.
- negative: mixed `f32`/`f64` arithmetic without explicit conversion.
- negative: dedicated complex opcode.
- negative: mixed-component complex pair representation.
- negative: invalid complex component index.
- negative: `restrict` on non-pointer type.
- negative: unknown qualifier atom.
- negative: incompatible qualifier redeclaration.
- negative: zero/negative fixed array length.
- negative: array element type `void`.
- negative: call argument arity/type mismatch.
- negative: VLA form at file scope.
- negative: flexible array member not final.
- negative: flexible array member as only named field.
- negative: object declaration of incomplete struct type.
- negative: second incompatible completion of same type name.
- negative: kind-mismatch completion (`struct` vs `union`).
- negative: array with incomplete element type.
- negative: flexible member declared in union.

## Module Level Metadata

Preliminary v1 listing of required module-level metadata.

Metadata tags may be attached:

- as prefix lines: `[!key: value]` preceding a declaration or instruction
- as inline annotations: `symbol ... [!key: value]` or `... instruction [!key: value]` at end of statement
- in list form: `[!key: value, !key: value, ...]`

All forms are equivalent in v1; inline form is recommended for conciseness when tagging individual symbols.

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
tag_def         = "tag" tag_id "=" "[" tag_list "]" ;
tag_use         = "[" tag_list "]" ;
tag_list        = tag_elem {"," tag_elem} ;
tag_elem        = tag_id [":" literal] ;
tag_id          = "!" id ;

global_symbol   = "@" (id | integer) ;
local_symbol    = "%" (id | integer) ;
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
symbol @0 : [6 i8] "hello"
symbol @main : () -> i32:
  call (ptr) -> i32 %0, @puts, (@0)
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

##### 1.3 Feature

Linkage determines the scope of symbol visibility across translation units. C99 defines three linkage classes: external (visible across translation units, default for functions and file-scope objects), internal (visible only within the translation unit, static at file scope), and no linkage (local variables, function parameters, labels). TIIR encodes linkage via the `!linkage` metadata tag on symbol declarations.

##### 1.3 Representative C Snippet

```c
int global_extern;         /* external linkage */
static int global_internal;/* internal linkage */

int func_extern(void) {     /* external linkage */
  static int local_static;  /* no linkage (allocated storage) */
  int local_auto;           /* no linkage (automatic storage) */
  return global_internal;
}

static int func_internal(void) { /* internal linkage */
  return global_extern;
}
```

##### 1.3 TIIR Canonical Form

```tiir
symbol @global_extern : i32 [!linkage: external];

symbol @global_internal : i32 [!linkage: internal];

symbol @func_extern : () -> i32:
  symbol %local_static : i32 [!linkage: internal];
  %0 = load @global_internal
  ret %0

symbol @func_internal : () -> i32 [!linkage: internal]:
  %0 = load @global_extern
  ret %0
```

##### 1.3 Grammar Productions Required

- symbol declaration with `!linkage` metadata tag
- valid linkage values: external, internal, none (or default if omitted)
- function and block-level symbol declarations

##### 1.3 In-Memory Nodes Required

- symbol node with linkage attribute
- linkage kind enum: EXTERNAL, INTERNAL, NONE
- mapping from C storage-class specifiers to linkage kind

##### 1.3 Semantic Validation Rules

- external linkage symbols are visible across translation units; all redeclarations must agree on external linkage
- internal linkage symbols are visible only in their translation unit; multiple internal symbols with same name in different translation units are distinct
- no-linkage symbols are local to their enclosing scope and have no cross-translation-unit visibility
- at most one external definition per symbol across all translation units (linker enforces)
- internal definitions do not conflict across translation units

##### 1.3 Lowering Notes (Target Independent)

Emit external symbols with global visibility directives compatible with target object format. Emit internal symbols with visibility restrictions to current translation unit only. Preserve no-linkage symbols as local scope entries with appropriate storage duration attributes.

##### 1.3 Test Coverage Status

Planned

- positive: external and internal symbols at file scope
- positive: function with internal linkage
- positive: static local variable within function
- positive: extern declaration in function scope
- negative: conflicting linkage redeclarations
- negative: multiple external definitions across modules (tested at linker level)

#### 1.4 Storage Duration: Static, Automatic, Allocated

##### 1.4 Feature

C99 object lifetime is governed by storage class and duration, which are semantically coupled. Static storage objects exist for the entire program; automatic storage objects exist for their enclosing block scope; allocated storage objects are created/destroyed via runtime APIs (`malloc`/`free`). TIIR encodes storage semantics in a unified `!storage` metadata tag on declarations. Allocation sites are marked with `!storage: alloc`, and deallocation sites (free/delete calls) are marked with `!storage: free` or `!storage: escape` to enable lifetime verification and leak detection passes. global storage space is implicitly `static`.
local storage space is implicitly `auto`. pointers allocated by malloc, calloc, and realloc are implicitly `alloc`. calls to free mark the pointer as
`free`, and returing the pointer from the function marks the pointer as `escape`. This requires no syntax.

##### 1.4 Representative C Snippet

```c
static int gs = 1;   /* static duration */

int f(void) {
  int a = 0;         /* automatic duration */
  int *p = malloc(sizeof(int)); /* allocated duration target object */
  *p = gs + a;
  free(p);
  return gs;
}

int *g(void) {
  return malloc(sizeof(int));
}
```

##### 1.4 TIIR Canonical Form

```tiir
symbol @gs : i32 = 1 [!linkage: internal];

symbol @f : () -> i32 [!linkage: external]:
  alloca i32 %a 
  store i32 %a, 0
  call (i64) -> ptr, %p, @malloc, (4)
  load i32 %0, @gs
  load i32 %1, %a
  add i32 %2, %0, %1
  store i32 %p, %2
  call (ptr) -> void @free, (%p)
  ret %0

symbol @g : () -> ptr [!storage: static, !linkage: external]:
  call (i64) -> ptr, %0, @malloc, (4)
  ret %0

```

##### 1.4 Grammar Productions Required

**optional**:

- symbol declarations with inline metadata tags
- metadata key/value entries for `!storage` (valid values: static, automatic, allocated)
- metadata key/value entries for deallocation point annotation (`!dealloc`)
- `alloca type %dst` for automatic/VLA stack allocation
- `dealloca type %src` for explicit end-of-lifetime stack release
- call/store/load forms sufficient to express allocated object usage through pointers

##### 1.4 In-Memory Nodes Required

- symbol storage-class attribute enum: STATIC, AUTOMATIC, ALLOCATED
- stack-allocation instruction nodes (`alloca`, `dealloca`) with scope/lifetime linkage
- instruction/result metadata attachment for allocated-storage values
- deallocation site tracking (free/delete call annotation) for lifetime analysis
- pointer/object-lifetime relationship tracking for analysis passes
- optional escape/liveness/must-free metadata hooks for allocated objects

##### 1.4 Semantic Validation Rules

- file-scope object definitions default to `!storage: static`
- block-scope non-static object declarations default to `!storage: automatic`
- block-scope `static` declarations map to `!storage: static`
- allocated-storage objects cannot be declared directly as symbols; they arise from allocation operations
- each allocation call must have a corresponding deallocation site or escape point for soundness checking
- each `alloca` must have a matching `dealloca` on all normal scope-exit paths for block-scoped lifetime modeling
- uses after `free` (marked `!dealloc`) are invalid at semantic-analysis level when detectable
- storage metadata on redeclarations must be compatible
- deallocation sites must reference previously allocated pointers

##### 1.4 Lowering Notes (Target Independent)

Lower `!storage: static` objects into module-level data definitions. Lower `!storage: automatic` objects into explicit stack regions using `alloca` at declaration point and `dealloca` at scope exit. Model `!storage: allocated` objects as heap references obtained from runtime/library calls. Preserve allocation/deallocation sites and annotate them with `!storage` and `!dealloc` metadata for later optimization and diagnostics. Enable static lifetime analysis to verify heap objects are deallocated before function return or marked as escaped, and stack objects are deallocated at scope end.

##### 1.4 Test Coverage Status

Planned

- positive: file-scope static object and block-scope automatic object
- positive: block-scope static local
- positive: heap allocation with deallocation before function return
- positive: allocation without deallocation marked as escaped
- negative: incompatible redeclaration of storage class
- negative: use-after-free (deallocation followed by use)
- negative: missing deallocation in configured leak-check mode (analysis warning)

#### 1.5 Scope: File, Block, Prototype

#### 1.5 Feature

Foe each entity that an identifier designates, the identifier is visible only
within a region of text, known as it's `scope`. C99 defines four kinds of scope:
function, file, block, and function prototype. A label name is the only kind of
identifier that has function scope. it can be used in a `goto` statement anywhere in
the function in which it appears.

For every other kind of identifier it's placement in the source text determines its scope.
if it appears outisde of any block or list of parameters, the identifier has `file` scope.
it is visible from the point of its declaration/definition until the end of the current
translation unit. if the identifier appears within a block, or a list of parameters, it has
`block` scope, it lasts until the termination of the associated block. if the identifier appears
within a function prototype, then it has `function prototype` scope, which ends at the
end of the function declarator.

If an identifier designates two different entities in the same name space, the scopes might
overlap. If so, the scope of one entity, the `inner` scope will be a strict subset of the scope
of the other entity, the `outer` scope. Within the inner scope, the identifier designates the
entity declared in the inner scope; the entity declared in the outer scope is `hidden` and not
visible within the inner scope.

#### 1.5 Representative C Snippet

```c
void f(int a /* function prototype scope */, int b);
/* a and b are `out of scope` or equivalently their `lifetime` has ended */

int c = 42; /* global scope */

int h(int i /* block scope */, int j) {
  int k = i * 2 + 4; /* block scope */

  {
    int k = k + j; 
    /* block scope, k `hides` 
      the outer definition of k while within 
      this inner scope
     */
    /* inner k's scope ends at this brace */
  }

  /* c is visible because it is declared within an outer scope */
  return f(k + c, k);
  /* i, j, k all fall out of scope at this brace */
}
```

#### 1.5 TIIR Canonical Form

```tiir
symbol @f : (i32, i32) -> (); [!linkage: external]

symbol @c : i32 = 42; [!linkage: internal]

symbol @h : (%i : i32, %j: i32) -> i32 [!linkage: external]:
%h_bb0:
  alloca i32 %k
  mul i32 %1, %i, 2
  add i32 %2, %1, 4
  store i32 %k, %2
  b %h_bb1

%h_bb1:
  load i32 %4, %k
  add i32 %5, %4, %j
  alloca i32 %k
  store i32 %k, %5
  dealloca i32 %k
  b %h_bb2

%h_bb2:
  load i32 %6, %k
  load i32 %7, @c
  add i32 %8, %6, %7
  call (i32, i32) -> i32 %9, @f, (%8, %6)
  dealloca i32 %k
  ret %9
```

#### 1.5 Grammar Productions Required

- global symbol declarations and definitions
- local symbol declarations and defintitions.
- stack lifetime instructions for local scope: `alloca` and `dealloca`

#### 1.5 In-Memory Nodes Required

- alloca instruction
- dealloca instruction
- global symbols
- local symbols

#### 1.5 Semantic Validation Rules

- file scope definitions are visible for the entire the translation unit from the point
  of their declaration/defintion.
- local scope definitions are visible for the lifetime of their declaring block.
- function parameters within a function declaration are only in scope within that declaration.
- every block-scoped automatic declaration lowered with `alloca` must be paired with
  `dealloca` when the declaration's scope ends.

#### 1.5 Lowering Notes (Target Independent)

- alloca allocates stack space per the target; dealloca releases stack space at block scope exit.
- global symbols are allocated in global data per the target. if the target has a zero initialized section,
  then zero initialized globals may be placed into such a section.
- the scope of local names within a function body needs to account for
  name shadowing. with alloca/dealloca we have clear scope boundaries.
  the name is introduced with alloca, and falls out of scope with dealloca.
  what if a name is introduced by another instruction?
  we could say names aren't allowed to be introduced by other instructions.
  we could say that the most recent definition is the visible one,
    that leaves open the question of a 'closing-brace' as it were.
  we could say names aren't allowed to be reused, and push that issue into
    the implementation of charon, to rename shadowed locals in order to
    lower into tiir.
  we can't say that a local name is only visible within the basic block that
    it is declared in, because a basic block != a 'block' in the C language.
    because control flow can go back into an outer block in c. whereas in
    tiir there is no such thing, the flow back into an outer scope is modeled
    by flowing into a new basic block. That is how the "after" is modeled.

- charon inserts `dealloca` on every scope exit edge (including branches that
  leave a scope) so block lifetime remains explicit in TIIR.

#### 1.5 Test Coverage Status

- positive: global symbol visible from global scope
- positive: global symbol visible from local scope
- positive: local symbol visible from local scope
- negative: global symbol not visible before declaration
- negative: local symbol not visible before declaration
- negative: local symbol not visible from global scope

#### 1.6 Identifier Namespaces

##### 1.6 Feature

C99 defines four distinct identifier namespaces (§6.2.3). An identifier can
designate entities in multiple namespaces simultaneously without conflict;
the namespace is resolved from context.

1. **Label names**: every function has exactly one label-name namespace.
   Labels are introduced by a labeled statement (`label:`) and referenced
   only by `goto`. They have function scope.

2. **Tags**: `struct`, `union`, and `enum` tags share one tag namespace per
   translation unit. A tag name introduced inside a block has block scope.

3. **Struct/union members**: each struct or union type has its own member
   namespace, distinct from every other struct/union and from ordinary
   identifiers.

4. **Ordinary identifiers**: all remaining identifiers — object names,
   function names, typedef names, enumeration constants — share one
   ordinary-identifier namespace per scope level.

TIIR collapses these four namespaces into two syntactic prefixes, because
the IR operates below the C type-name surface:

- `@name` — global ordinary identifiers (file-scope objects, functions,
  string constants, etc.).
- `%name` — local ordinary identifiers (parameters, virtual registers,
  basic-block labels within a function body).

TIIR additionally supports an explicit struct-member-tag namespace for
structural member labels. Member tags use `%id` syntax but are local to the
defining struct type and are only valid in structural contexts (type
descriptors and `gep` member selection).

Struct/union member names appear only inside type descriptors and are
unambiguous by position/tag; they do not share ordinary identifier scope.
Tag names are represented as named type definitions and also do not share
ordinary identifier scope.

##### 1.6 Representative C Snippet

```c
struct Point { int x; int y; };  /* tag namespace: Point, member namespace: x, y */

typedef struct Point Point;      /* ordinary namespace: Point (typedef) */

int x = 1;                       /* ordinary namespace: x (object) */

int f(int x) {                   /* ordinary namespace: f (function), x (param) */
  struct Point p;                /* ordinary namespace: p; tag namespace: Point */
  p.x = x;                      /* member x of struct Point, not ordinary x */

  goto done;                     /* label namespace: done */
done:
  return p.x + x;
}
```

##### 1.6 TIIR Canonical Form

```tiir
/* tag namespace: struct Point becomes a named type descriptor */
type @Point = struct { %x: i32, %y: i32 };

symbol @x : i32 = 1 [!linkage: external];

symbol @f : (%x : i32) -> i32 [!linkage: external]:
%f_bb0:
  alloca @Point %p
  gep @Point %0, %p, %x      /* pointer to tagged member %x */
  store i32 %0, %x
  b %done

%done:
  gep @Point %2, %p, %x      /* pointer to tagged member %x */
  load i32 %3, %2
  add i32 %4, %3, %x
  ret %4
```

##### 1.6 Grammar Productions Required

- named type definition form: `type @id = struct { struct_elem_list };` (or `union { ... }` / `enum { ... }` for the respective C type kinds)
- struct element form: `["%" id ":"] type` (member tag optional)
- `gep` instruction form: `gep type dest, srcPtr, member_selector` where `member_selector` is either an integer index or `%id`; `%id` form is valid iff `type` is structural and the selected member has a tag
- basic-block label production (already required for 1.5)
- ordinary global/local symbol productions (already required for 1.1–1.5)

##### 1.6 In-Memory Nodes Required

- named type table (maps type name `@id` to struct/union type descriptor)
- struct/union type descriptor: ordered member list with types and optional member tags
- per-struct member-tag table mapping `%id` -> member index
- `gep` instruction node: base-type reference, destination symbol, source pointer operand, and selector operand (integer index or member tag)
- no separate label-namespace node is needed: basic-block label entries in the
  function body serve this role

##### 1.6 Semantic Validation Rules

- within a single scope level, two ordinary identifiers with the same name are
  a conflict; TIIR enforces this via unique `@`/`%` symbol names per scope
- tag names (`type @id`) must be unique in the named-type table; redefinition
  with the same layout is a no-op (tentative); incompatible redefinition is an
  error
- member tags within a single struct/union type must be unique; member tags
  across different struct/union types may coincide without conflict
- `gep ... , %id` is valid only when the base type is structural and `%id`
  resolves in that structural type's member-tag namespace
- `gep ... , %id` resolution is performed in the struct scope (not function/
  block scope), even though tag spelling uses `%` prefix
- basic-block labels within one function must be unique; the same label name
  may appear in different functions without conflict
- a `%label` used as a branch target must be declared as a label within the
  same function body

##### 1.6 Lowering Notes (Target Independent)

C's four-namespace system is resolved entirely by the frontend (charon) before
emission. By the time TIIR is produced:

- All tag names that survive as allocatable types are emitted as `type @id`
  definitions.
- All ordinary names have been uniquified per scope and assigned `@` or `%`
  prefix identifiers; shadowed names are renamed to fresh temporaries.
- Member tags may be retained in structural type descriptors and used directly
  by `gep` selector resolution; numeric selector form remains valid.
- Label names are lowered to basic-block labels; `goto` is lowered to an
  unconditional branch instruction.

##### 1.6 Test Coverage Status

Planned

- positive: struct type definition with member access by index
- positive: ordinary identifier at file scope and shadowed local with same name
- positive: `goto` lowered to unconditional branch using basic-block label
- negative: duplicate ordinary identifier in same scope
- negative: duplicate member name within one struct type
- negative: branch to undefined basic-block label within function
- negative: incompatible struct redefinition

### 2. Preprocessing and Lexical Surface

This entire section is resolved within charon (the frontend). TIIR is a post-preprocessing, post-lexing IR and is concerned only with C99 semantics, not C99 syntax. Trigraphs, digraphs, universal character names, line splicing, comments, macro expansion, conditional inclusion, and header search are all consumed and discarded before TIIR emission. No TIIR grammar productions or in-memory nodes are required for any item in this section.

### 3. Type System Core

#### 3.1 Void

##### 3.1 Feature

C99 `void` is an incomplete type with no values. It is used in three contexts:

1. **Function return type**: `void f(void)` — the function produces no value.
2. **Parameter list**: `f(void)` — the function accepts no arguments (distinct from `f()` which leaves parameters unspecified).
3. **Generic pointer**: `void *` — a pointer to an object of unknown type, compatible with any object pointer type.

TIIR represents void as the keyword `void`. A function returning void has return type `void`. A void parameter list is represented as an empty parameter list `()`. A generic pointer is `ptr` (the opaque pointer type, already locked in v1 scope).

##### 3.1 Representative C Snippet

```c
void say_hello(void);       /* void return, void params */

void say_hello(void) {      /* definition */
    puts("hello");
}

void *memcopy(void *dst, const void *src, int n); /* void* params and return */
```

##### 3.1 TIIR Canonical Form

```tiir
symbol @say_hello : () -> void [!linkage: external];

symbol @say_hello : () -> void [!linkage: external]:
  call (ptr) -> i32 %0, @puts, (@str_hello)
  ret

symbol @memcopy : (ptr, ptr, i32) -> ptr [!linkage: external];
```

##### 3.1 Grammar Productions Required

- `void` keyword as a return type in function signatures
- `ret` with no operand for void-returning functions
- `ptr` as the opaque pointer type (already in v1 scope)
- empty parameter list `()` to denote no parameters

##### 3.1 In-Memory Nodes Required

- `void` type kind in the type enum
- function type node that permits `void` as return type
- `ret` instruction node with optional operand (absent for void return)

##### 3.1 Semantic Validation Rules

- a `ret` with no operand is only valid in a function whose return type is `void`
- a `ret` with an operand is only valid in a function whose return type is not `void`
- `void` may not appear as the type of an object symbol (no `symbol @x : void`)
- `void` may not appear as an element type in a struct, union, or array
- `ptr` is the correct type for `void *`; no separate `void *` type node is needed

##### 3.1 Lowering Notes (Target Independent)

Functions with void return type emit no return value. The `ret` instruction for void functions lowers to a bare return with no register operand. `void *` parameters and return values are lowered uniformly as `ptr`, consistent with the opaque-pointer model already adopted in v1.

##### 3.1 Test Coverage Status

Planned

- positive: void-return function with no parameters declared and defined
- positive: void-return function body ending with bare `ret`
- positive: `ptr` parameter representing a `void *` argument
- negative: `ret` with operand in a void-returning function
- negative: `ret` with no operand in a non-void-returning function
- negative: object symbol declared with type `void`

#### 3.2 Character Types

##### 3.2 Feature

C99 defines three character types: `char`, `signed char`, and `unsigned char`. Whether plain `char` is signed or unsigned is implementation-defined. `wchar_t`, `char16_t`, and `char32_t` are typedef aliases for implementation-defined integer types. All character types are lowered to integer types in TIIR; no distinct character type keyword exists in the IR.

| C99 type       | TIIR type | Notes                                      |
|----------------|-----------|--------------------------------------------|
| `char`         | `i8`      | signedness resolved by target/ABI          |
| `signed char`  | `i8`      | explicitly signed 8-bit integer            |
| `unsigned char`| `u8`      | explicitly unsigned 8-bit integer          |
| `wchar_t`      | `i32`     | typical; target-dependent width            |
| `char16_t`     | `u16`     | UTF-16 code unit                           |
| `char32_t`     | `u32`     | UTF-32 code point                          |

String literals are lowered to `[N i8]` or `[N u8]` array symbols as appropriate.

##### 3.2 Representative C Snippet

```c
void f(void) {
  char c = 'A';
  unsigned char uc = 0xFF;
  signed char sc = -1;
  const char *s = "hi";
}
```

##### 3.2 TIIR Canonical Form

```tiir
symbol @str_hi : [3 i8] = "hi\00" [!linkage: internal];

symbol @f : () -> void [!linkage: external]:
  alloca i8 %c
  store i8 %c, 65
  alloca u8 %uc
  store u8 %uc, 255
  alloca i8 %sc
  store i8 %sc, -1
  alloca ptr %s
  store ptr %s, @str_hi
  ret
```

##### 3.2 Grammar Productions Required

- `i8` and `u8` as integer type keywords (covered by 3.3 integer types)
- string literal constant lowered to a named array symbol (covered by existing string-constant support)
- no new type keywords required; character types map entirely onto existing integer type tokens

##### 3.2 In-Memory Nodes Required

- no new type nodes; `i8`/`u8` are integer type instances
- the mapping from C character types to TIIR integer types is a charon frontend concern, not an IR node concern

##### 3.2 Semantic Validation Rules

- `i8` and `u8` obey the same arithmetic and overflow rules as all integer types in TIIR
- signedness is explicit at the IR level; charon is responsible for resolving the implementation-defined signedness of plain `char` before emission

##### 3.2 Lowering Notes (Target Independent)

All character type lowering is performed by charon before TIIR emission. The IR sees only `i8`, `u8`, `i16`, `u16`, `i32`, `u32` etc. String literals are emitted as named array symbols with a null terminator appended. No character-type-specific instruction semantics exist in TIIR.

##### 3.2 Test Coverage Status

Planned

- positive: `i8` and `u8` object allocations and stores with character-range literals
- positive: string literal emitted as `[N i8]` array symbol with null terminator
- positive: `ptr` symbol pointing to a string literal array

#### 3.3 Integer Types

##### 3.3 Feature

C99 integer types have implementation-defined widths constrained by the standard's minimum-range requirements. TIIR uses explicit-width integer types so that the IR is width-unambiguous regardless of target. Charon resolves all C integer type names to their concrete widths for the target ABI before emission.

| C99 type                        | TIIR type   | Width            | Signedness |
|---------------------------------|-------------|------------------|------------|
| `signed char`                   | `i8`        | 8-bit            | signed     |
| `unsigned char`                 | `u8`        | 8-bit            | unsigned   |
| `short` / `signed short`        | `i16`       | 16-bit           | signed     |
| `unsigned short`                | `u16`       | 16-bit           | unsigned   |
| `int` / `signed int`            | `i32`       | 32-bit           | signed     |
| `unsigned int`                  | `u32`       | 32-bit           | unsigned   |
| `long` / `signed long`          | `i32`/`i64` | target-dependent | signed     |
| `unsigned long`                 | `u32`/`u64` | target-dependent | unsigned   |
| `long long` / `signed long long`| `i64`       | 64-bit           | signed     |
| `unsigned long long`            | `u64`       | 64-bit           | unsigned   |
| `intptr_t` / `ptrdiff_t`        | `i64`       | 64-bit (x86-64)  | signed     |
| `uintptr_t` / `size_t`          | `u64`       | 64-bit (x86-64)  | unsigned   |

TIIR integer type keywords follow the pattern `i<width>` for signed and `u<width>` for unsigned, where width is a positive power-of-two number of bits.

##### 3.3 Representative C Snippet

```c
void f(void) {
  int a = 1;
  unsigned int b = 2u;
  long long c = -1LL;
  unsigned long long d = 0xFFFFFFFFFFFFFFFFULL;
  short e = 32767;
}
```

##### 3.3 TIIR Canonical Form

```tiir
symbol @f : () -> void [!linkage: external]:
  alloca i32 %a
  store i32 %a, 1
  alloca u32 %b
  store u32 %b, 2
  alloca i64 %c
  store i64 %c, -1
  alloca u64 %d
  store u64 %d, 18446744073709551615
  alloca i16 %e
  store i16 %e, 32767
  ret
```

##### 3.3 Grammar Productions Required

- integer type keywords: `i8`, `i16`, `i32`, `i64`, `u8`, `u16`, `u32`, `u64` (and any further power-of-two widths as needed)
- signed and unsigned integer literal tokens (decimal, hex `0x`, octet `0o`, binary `0b`)
- integer literal as an operand in `store`, `add`, `sub`, etc.

##### 3.3 In-Memory Nodes Required

- integer type node with width (bits) and signedness fields
- integer literal operand node with value and type

##### 3.3 Semantic Validation Rules

- integer type width must be one of 1, 8, 16, 32, 64.
- integer literal value must be representable in the declared type width and signedness; out-of-range literals are a parse/validation error
- signed overflow in constant expressions is a validation error; unsigned wrap-around is well-defined
- mixed-width operands to arithmetic instructions are not permitted; charon must emit explicit cast/extend/truncate instructions before use

##### 3.3 Lowering Notes (Target Independent)

All C integer type names are resolved to explicit TIIR width types by charon, using the target ABI's type-width table. No implicit promotion or widening occurs at the TIIR level; all integer promotions required by C99 (e.g., `short` promoted to `int` in arithmetic) must be made explicit by charon via cast instructions before emission.

##### 3.3 Test Coverage Status

Planned

- positive: allocation and store for each integer type width and signedness
- positive: hex and binary integer literals
- positive: maximum and minimum representable values for each type
- negative: integer literal out of range for declared type
- negative: mixed-width operands to an arithmetic instruction without explicit cast

#### 3.4 Boolean Type

##### 3.4 Feature

C99 introduces `_Bool` as an integer type constrained to values `0` and `1`.
Including `<stdbool.h>` provides the macro alias `bool` and constants `true`
and `false`, but TIIR models only the semantic type/value pair, not macro
surface syntax.

TIIR represents Boolean values as `i1`.

- `false` lowers to integer literal `0` of type `i1`.
- `true` lowers to integer literal `1` of type `i1`.
- Any scalar-to-bool conversion lowers to an explicit compare against zero,
  producing `i1`.

##### 3.4 Representative C Snippet

```c
#include <stdbool.h>

bool is_nonzero(int x) {
  bool b = x;
  return b;
}

bool both(bool a, bool c) {
  return a && c;
}
```

##### 3.4 TIIR Canonical Form

```tiir
symbol @is_nonzero : (%x: i32) -> i1 [!linkage: external]:
  ne i32 %0, %x, 0
  ret %0

symbol @both : (%a: i1, %b: i1) -> i1 [!linkage: external]:
  and i1 %0, %a, %c
  ret %0
```

##### 3.4 Grammar Productions Required

- `i1` integer type keyword (Boolean semantic type)
- comparison instructions producing `i1` (for example `eq`, `ne`, `lt`, `le`,
  `gt`, `ge`)
- logical/bitwise-on-boolean instructions over `i1` (`and`, `or`, `xor`,
  `not`)

##### 3.4 In-Memory Nodes Required

- integer type node instance for width 1 (`i1`)
- comparison instruction nodes with result type fixed to `i1`
- Boolean constants represented as typed integer literals (`0`/`1` on `i1`)

##### 3.4 Semantic Validation Rules

- only `0` and `1` are valid immediate literal values for `i1`
- arithmetic instructions (`add`, `sub`, `mul`, `div`, etc.) are invalid on `i1`
- `and`/`or`/`xor` over `i1` are valid and preserve `i1`
- branch conditions must be `i1`; non-`i1` conditions require an explicit
  compare before branching
- scalar-to-bool conversion must be explicit in TIIR (typically `ne T %dst, %v, 0`)

##### 3.4 Lowering Notes (Target Independent)

Charon resolves `_Bool`/`bool` to TIIR `i1` before emission. C's truthiness
rules (non-zero is true) are lowered explicitly via compare-to-zero operations.
Logical `&&` and `||` short-circuit behavior is preserved by control flow
structure in charon lowering; final Boolean results in TIIR remain `i1`.

##### 3.4 Test Coverage Status

Planned

- positive: `_Bool`/`bool` variable initialization from integer expression via compare
- positive: function returning `i1`
- positive: branch using `i1` condition
- negative: arithmetic instruction applied directly to `i1`
- negative: branch on non-`i1` value without explicit compare
- negative: `i1` literal other than `0` or `1`

#### 3.5 Enumerated Types

##### 3.5 Feature

C99 enumerations define a set of named integer constants. Enumerator values are
compile-time integer constants with implicit increment semantics unless explicit
initializers are provided.

TIIR treats enum values as integers after frontend lowering. Charon resolves
enumerator names and values before emission. The underlying storage type is a
concrete integer type selected by the target ABI policy (commonly `i32` for C99
toolchains unless constrained otherwise).

Enumerator identifiers are also introduced into TIIR's global ordinary-identifier
namespace, matching C behavior. In instruction operands, enumerators are referenced
as global symbols using `@` (for example: `@RED`, `@GREEN`).

For readability and debug fidelity, TIIR may optionally preserve an enum type
definition using concise `@`-prefixed enumerators in the locked form
`enum { @ELEM = value, ... }`, while instructions and data operations still use
the resolved integer type.

##### 3.5 Representative C Snippet

```c
enum Color {
  RED,
  GREEN = 4,
  BLUE
};

enum Color pick(int v) {
  if (v == 0) return RED;
  if (v == 1) return GREEN;
  return BLUE;
}
```

##### 3.5 TIIR Canonical Form

```tiir
type @Color = enum { @RED = 0, @GREEN = 4, @BLUE = 5 };

symbol @pick : (%v: i32) -> i32 [!linkage: external]:
%pick_bb0:
  eq i32 %0, %v, @RED
  br %0, %pick_bb_red, %pick_bb1

%pick_bb1:
  eq i32 %1, %v, @GREEN
  br %1, %pick_bb_green, %pick_bb_blue

%pick_bb_red:
  ret @RED

%pick_bb_green:
  ret @GREEN

%pick_bb_blue:
  ret @BLUE
```

##### 3.5 Grammar Productions Required

- enum type definition form: `type @id = enum { enum_elem_list };`
- enum element form: `@id ["=" integer_literal]`
- trailing comma support in enum element list (optional but recommended)
- global enum symbol reference in operands: `@id`
- no distinct enum runtime operand type is required; enum operands are integer-typed

##### 3.5 In-Memory Nodes Required

- optional enum type descriptor node:
  - type name
  - ordered enumerator list `(name, value)`
  - chosen underlying integer type
- global symbol entries for enumerators (name, integer value)
- mapping table from enumerator name to global symbol and resolved integer value

##### 3.5 Semantic Validation Rules

- each enumerator value must be an integer constant expression resolved at
  compile time by charon
- implicit enumerator values increment from the previous resolved value by `+1`
- duplicate enumerator names in the same enum are invalid
- resolved enumerator value must fit in the selected underlying integer type
- enumerator identifiers are emitted as global symbols and must be referenced as `@id`
- duplicate enumerator names across different enums in the same translation unit
  are invalid because they share the global ordinary-identifier namespace
- TIIR instruction typing uses the underlying integer type only (no distinct
  enum runtime type semantics in v1)

##### 3.5 Lowering Notes (Target Independent)

Enum declarations are resolved completely in charon. IR emission keeps enumerator
names as global symbols for readability and C namespace fidelity, while execution
semantics remain integer-based. Backends may fold `@ENUM_NAME` constants to immediates,
but source-level TIIR keeps symbolic references.

##### 3.5 Test Coverage Status

Planned

- positive: enum with implicit and explicit enumerator values
- positive: function returning `@ENUM_NAME` global symbols
- positive: enum symbols used in compare and branch lowering
- negative: enum identifier used without `@` in instruction operand
- negative: duplicate enumerator name
- negative: explicit enumerator value outside representable range of selected
  underlying type
- negative: non-constant enumerator initializer

#### 3.6 Floating Types

##### 3.6 Feature

C99 floating semantic categories are lowered to explicit TIIR floating-width
types.

- `float` lowers to `f32`
- `double` lowers to `f64`
- `long double` lowers to target policy in charon (v1 default: `f64` unless a
  target profile explicitly requires a wider representation)

In TIIR v1, only `f32` and `f64` are required floating types.

`f16` and `f128` are reserved as future extensions and are not required for v1
parsing/validation/execution.

##### 3.6 Representative C Snippet

```c
float addf(float a, float b) {
  return a + b;
}

double hyp2(double x, double y) {
  return x * x + y * y;
}
```

##### 3.6 TIIR Canonical Form

```tiir
symbol @addf : (%a: f32, %b: f32) -> f32 [!linkage: external]:
  add f32 %0, %a, %b
  ret %0

symbol @hyp2 : (%x: f64, %y: f64) -> f64 [!linkage: external]:
  mul f64 %0, %x, %x
  mul f64 %1, %y, %y
  add f64 %2, %0, %1
  ret %2
```

##### 3.6 Grammar Productions Required

- floating type keywords: `f32`, `f64`
- floating literal tokens (decimal with optional exponent; hexadecimal float
  literal support optional for v1)
- arithmetic instructions over floating operands (`add`, `sub`, `mul`, `div`)
- floating compare instructions producing `i1` (`feq`, `fne`, `flt`, `fle`,
  `fgt`, `fge`)

##### 3.6 In-Memory Nodes Required

- floating type node with width field (v1 allowed widths: 32, 64)
- floating literal operand node with parsed value and type
- floating compare instruction nodes producing `i1`

##### 3.6 Semantic Validation Rules

- in v1, only `f32` and `f64` are valid floating types
- `f16` and `f128` tokens are rejected in v1 unless an extension/profile gate is enabled
- arithmetic instruction operands must have identical floating type width
- integer/float mixed operations require explicit conversion instructions before use
- branch conditions remain `i1`; float conditions must be formed by explicit
  `f*cmp` instruction first

##### 3.6 Lowering Notes (Target Independent)

Charon resolves C floating surface types to TIIR `f32`/`f64` according to target
ABI policy. C implicit promotions (for example `float` to `double` in variadic
contexts) are made explicit before TIIR emission via conversion instructions.
TIIR execution semantics use explicit widths only; no implicit widening occurs
inside IR instructions.

##### 3.6 Test Coverage Status

Planned

- positive: `f32` arithmetic function
- positive: `f64` arithmetic function
- positive: explicit float comparison to produce `i1` for branch
- negative: use of `f16` in v1 without extension gate
- negative: use of `f128` in v1 without extension gate
- negative: mixed `f32`/`f64` arithmetic without explicit conversion

#### 3.7 Complex Types

##### 3.7 Feature

C99 complex types (`float _Complex`, `double _Complex`, `long double _Complex`)
represent values with real and imaginary components.

TIIR v1 does not introduce dedicated complex primitive types or complex
instructions. Complex values are represented as aggregates of two floating
scalars and complex operations are lowered in charon before TIIR emission.

- `float _Complex` lowers to `struct { f32, f32 }`
- `double _Complex` lowers to `struct { f64, f64 }`
- `long double _Complex` lowers by target policy (v1 default follows `f64`-pair
  representation unless a target profile specifies otherwise)

##### 3.7 Representative C Snippet

```c
double _Complex addc(double _Complex a, double _Complex b) {
  return a + b;
}
```

##### 3.7 TIIR Canonical Form

```tiir
type @c64 = struct { f64, f64 };

symbol @addc : (%a: @c64, %b: @c64) -> @c64 [!linkage: external]:
%addc_bb0:
  alloca @c64 %out

  gep @c64 %a_re_ptr, %a, 0
  gep @c64 %a_im_ptr, %a, 1
  gep @c64 %b_re_ptr, %b, 0
  gep @c64 %b_im_ptr, %b, 1
  gep @c64 %o_re_ptr, %out, 0
  gep @c64 %o_im_ptr, %out, 1

  load f64 %a_re, %a_re_ptr
  load f64 %a_im, %a_im_ptr
  load f64 %b_re, %b_re_ptr
  load f64 %b_im, %b_im_ptr

  add f64 %o_re, %a_re, %b_re
  add f64 %o_im, %a_im, %b_im

  store f64 %o_re_ptr, %o_re
  store f64 %o_im_ptr, %o_im
  ret %out
```

##### 3.7 Grammar Productions Required

- no new primitive complex type keywords in v1
- aggregate type forms already defined for structs are sufficient
- existing `gep`, `load`, `store`, and floating arithmetic instructions are
  sufficient for lowered complex operations

##### 3.7 In-Memory Nodes Required

- no dedicated complex type node in v1
- struct type descriptor reused for complex pair representation
- optional metadata or type alias tagging to mark complex-origin types for debug
  and diagnostics (for example via tags)

##### 3.7 Semantic Validation Rules

- in v1, complex operations must not appear as dedicated IR opcodes
- complex-origin values must be represented as two-element float aggregates with
  homogeneous component type (`f32` pair or `f64` pair)
- complex arithmetic emitted by charon must preserve C99 semantic formulas:
  - add/sub: component-wise
  - mul: `(ar*br - ai*bi, ar*bi + ai*br)`
  - div: lowered via target-stable formula or helper call
- component extraction and insertion must use valid `gep` indices (`0` real,
  `1` imaginary)

##### 3.7 Lowering Notes (Target Independent)

Charon fully lowers complex expressions into scalar float operations (or
well-defined runtime helper calls when needed for correctness/performance).
TIIR remains explicit and target-neutral, with no hidden complex semantics.
This keeps parser and verifier scope small while preserving C99 behavior.

##### 3.7 Test Coverage Status

Planned

- positive: complex add lowered to pairwise float adds
- positive: complex multiply lowered to scalar float formula
- positive: real/imag component access via `gep` indices 0 and 1
- negative: dedicated complex opcode in v1 module
- negative: mixed-component pair type (for example `struct { f32, f64 }`) used
  as lowered complex representation
- negative: invalid component index for complex pair access

#### 3.8 Qualified Types: const, volatile, restrict

##### 3.8 Feature

C99 type qualification is represented in TIIR via existing metadata tags,
not via separate type keywords.

- `const`
- `volatile`
- `restrict`

The canonical encoding uses `!qualifiers` with a list of qualifier atoms.

- `symbol @x : i32 [!qualifiers: const]`
- `symbol %p : ptr [!qualifiers: restrict]`
- `symbol @reg : i32 [!qualifiers: volatile]`
- `symbol %q : ptr [!qualifiers: const, restrict]`

Qualifier semantics are attached to the declaration site (symbol/parameter/
field) and interpreted by verifier/lowering passes.

##### 3.8 Representative C Snippet

```c
extern volatile int mmio_reg;

void copy(int *restrict dst, const int *restrict src) {
  *dst = *src;
}
```

##### 3.8 TIIR Canonical Form

```tiir
symbol @mmio_reg : i32 [!linkage: external, !qualifiers: volatile];

symbol @copy : (
  %dst: ptr [!qualifiers: restrict],
  %src: ptr [!qualifiers: const, restrict]
) -> void [!linkage: external]:
%copy_bb0:
  load i32 %0, %src
  store i32 %dst, %0
  ret
```

##### 3.8 Grammar Productions Required

- reuse generic tag-list grammar already defined (`[!key: value, ...]`)
- reserve `!qualifiers` metadata key with accepted atoms:
  - `const`
  - `volatile`
  - `restrict`
- allow qualifiers on symbol declarations, parameters, and aggregate fields

##### 3.8 In-Memory Nodes Required

- qualifier bitset on type-bearing declarations (CONST, VOLATILE, RESTRICT)
- metadata parser support to decode `!qualifiers` into the qualifier bitset
- optional field-level qualifier storage for struct/union members

##### 3.8 Semantic Validation Rules

- unknown qualifier atoms under `!qualifiers` are invalid
- duplicate qualifier atoms in one qualifier set are invalid
- `restrict` is valid only for pointer-typed declarations
- `const` prohibits stores through the qualified declaration/reference
- `volatile` marks accesses as side-effectful and non-elidable by optimization
- redeclarations must have compatible qualifier sets for the same declared entity

##### 3.8 Lowering Notes (Target Independent)

Charon is responsible for mapping C qualifier syntax onto TIIR metadata tags.
TIIR keeps qualifier semantics explicit at declaration sites through
`!qualifiers`, enabling verifier checks and downstream optimization constraints
without introducing additional type-surface syntax.

`volatile` affects optimization legality, not value typing. `const` and
`restrict` primarily constrain aliasing/mutation behavior and are consumed by
analysis/lowering passes.

##### 3.8 Test Coverage Status

Planned

- positive: volatile global symbol declaration and volatile load/store handling
- positive: restrict-qualified pointer parameters in function signature
- positive: const-qualified pointer source operand rejected on store-through
- negative: `restrict` on non-pointer type
- negative: unknown qualifier atom in `!qualifiers`
- negative: incompatible qualifier redeclaration

#### 3.9 Derived Types: Pointers, Arrays, Functions

##### 3.9 Feature

C99 derived types are mapped into TIIR with explicit, compact forms:

- pointers use the opaque keyword `ptr`
- arrays use `[N T]` where `N` is a compile-time integer and `T` is element type
- runtime-sized arrays use `[* T]` in the restricted contexts where C99 allows
  variable length arrays
- functions use `(param_types) -> return_type`

Structural aggregate elements may carry optional local member tags:

- `struct { %x: i32, %y: i32 }`
- `struct { i32, %count: i32 }` (mixed tagged/untagged)

This keeps type spelling simple while preserving required semantics in
instruction typing and validation.

Pointer pointee details are not carried in the `ptr` type itself; pointee
interpretation is supplied by the instruction that consumes the pointer
(for example `load T`, `store T`, `gep BaseType`).

Parenthesized syntax in instruction operands is treated as a variadic operand
pack and is not a general-purpose type or literal form.

Instruction operand rules:

- `call` always receives arguments as one parenthesized operand pack
- any instruction requiring variable arity receives the variable portion as a
  parenthesized operand pack

C99 variable length arrays (VLAs) are supported only for automatic storage
within function bodies in v1. They are lowered to `alloca` with runtime extent,
not emitted as global or static object types.

C99 flexible array members are supported in struct definitions with the
following constraints:

- the flexible member must be the last member in the struct
- the flexible member may not be the only named member in the struct

##### 3.9 Representative C Snippet

```c
int g_arr[4] = {1, 2, 3, 4};

int inc(int x) { return x + 1; }

int first(int *p) {
  return p[0];
}
```

##### 3.9 TIIR Canonical Form

```tiir
symbol @g_arr : [4 i32] = [1, 2, 3, 4];

symbol @inc : (%x: i32) -> i32 [!linkage: external]:
  add i32 %0, %x, 1
  ret %0

symbol @first : (%p: ptr) -> i32 [!linkage: external]:
%first_bb0:
  gep [4 i32] %0, %p, 0
  load i32 %1, %0
  ret %1

symbol @sum2 : (%a: i32, %b: i32) -> i32 [!linkage: external]:
  add i32 %0, %a, %b
  ret %0

symbol @use_sum2 : (%x: i32, %y: i32) -> i32 [!linkage: external]:
  call (i32, i32) -> i32 %0, @sum2, (%x, %y)
  ret %0
```

##### 3.9 Grammar Productions Required

- pointer type keyword: `ptr`
- array type form: `"[" integer_literal type "]"`
- runtime/flexible array form: `"[" "*" type "]"`
- array literal initializer form: `"[" [literal {"," literal}] "]"`
- function type form: `"(" [param_type_list] ")" "->" type`
- call argument operand form: parenthesized operand pack
- var-arity instruction operand form: parenthesized operand pack
- optional vararg marker in function types is deferred to later section/profile
- `gep type dest, srcPtr, selector` is used for array/aggregate element
  addressing, where `selector` is integer index or `%id` (for tagged structural
  members)

##### 3.9 In-Memory Nodes Required

- opaque pointer type node (single canonical `ptr` type)
- array type node: `(length, element_type)`
- function type node: `(parameter_types, return_type)`
- variadic operand-pack node: ordered operand list (instruction-specific, not a type)
- optional function-type attribute flags (for example vararg) for future extension

##### 3.9 Semantic Validation Rules

- `ptr` carries no implicit pointee type; pointer-consuming instructions must
  supply explicit type context
- array element type must be complete and non-`void`
- function return type may be `void` or a non-function type
- function parameter types may not be raw array/function forms at call boundary;
  charon must lower C parameter decay to `ptr` before TIIR emission
- direct calls to `@function` must match declared function type arity and types
- fixed-size array length `N` must be a positive compile-time integer
- `[* T]` (VLA form) is valid only for automatic storage declarations lowered
  to runtime-extent `alloca` within function scope
- each VLA `alloca` must have a matching `dealloca` at the end of the
  declaration's scope extent
- `[* T]` is invalid for file-scope/global symbols
- `[* T]` as a struct member is valid only as a flexible array member when it
  is the last member and the struct has at least one other named member
- operand-pack element count and types must match the consumer instruction
  context (for example call parameter list)
- `ret` returns either no operand (`ret`) or a single typed operand (`ret %v`)
- aggregate index access continues to use `gep` with integer index for
  structs, unions, and arrays; tagged structural members may alternatively use
  `%id` selector form

##### 3.9 Lowering Notes (Target Independent)

Charon resolves C declarator complexity into normalized TIIR types before
emission. In particular:

- C array-to-pointer decay is made explicit as `ptr` in expression/call contexts
- C function-to-pointer decay is made explicit as `ptr` when a function value is
  used as data
- memory operations remain typed at instruction sites (`load T`, `store T`,
  `gep BaseType`) to preserve correctness despite opaque pointers
- C99 VLA declarations are lowered to runtime-extent stack allocation
  (`alloca`) and paired `dealloca` at scope end
- flexible array members are represented structurally and participate in `gep`
  element addressing as trailing members
- call lowering always emits one operand-pack argument group, even for zero/one
  argument calls (`()`, `(arg0,)`)

##### 3.9 Test Coverage Status

Planned

- positive: global array declaration with constant initializer
- positive: pointer parameter plus `gep`/`load` element access
- positive: direct function definition and call type-check
- positive: call argument operand pack matches function parameter types
- positive: local VLA lowered to runtime-extent `alloca`
- positive: local VLA lowered to `alloca` and released by `dealloca` at scope exit
- positive: struct with trailing flexible array member and at least one prior
  named member
- negative: zero or negative array length
- negative: array element type `void`
- negative: call argument count/type mismatch versus function signature
- negative: VLA form used at file scope/global symbol
- negative: flexible array member not in final position
- negative: flexible array member as only named struct member
- negative: call operand-pack arity/type mismatch with function signature

#### 3.10 Incomplete vs Completed Types

##### 3.10 Feature

C99 distinguishes incomplete and complete types. TIIR v1 keeps this distinction
only where it affects structural type declarations and validation.

Type status in TIIR v1:

- always incomplete: `void`
- may be incomplete then completed: named `struct`/`union` types
- always complete at emission time: scalar integer/float/bool types,
  function types, and fixed-size arrays `[N T]`
- context-dependent completion: runtime-sized arrays `[* T]` (VLA/flexible form)

Runtime-sized array form `[* T]` is permitted only in restricted C99-compatible
contexts: automatic-storage VLAs and trailing flexible array members.

##### 3.10 Representative C Snippet

```c
struct Node;                 /* incomplete */
extern struct Node *head;    /* pointer to incomplete is valid */

struct Node {                /* completion */
  int value;
  struct Node *next;
};

struct Node g_node;          /* requires complete type */
```

##### 3.10 TIIR Canonical Form

```tiir
type @Node = struct;

symbol @head : ptr [!linkage: external];

type @Node = struct { %value: i32, %next: ptr };

symbol @g_node : @Node [!linkage: external];
```

##### 3.10 Grammar Productions Required

- incomplete structural declaration form: `type @id = struct;` and
  `type @id = union;`
- completed structural definition form: `type @id = struct { ... };` and
  `type @id = union { ... };`
- named type references in declarations must resolve against the type table

##### 3.10 In-Memory Nodes Required

- named type table entry with completion state:
  - DECLARED_INCOMPLETE
  - DEFINED_COMPLETE
- structural payload attached only in DEFINED_COMPLETE state
- fixup/reference list for declarations that mention a named type before
  completion

##### 3.10 Semantic Validation Rules

- `void` is incomplete and cannot be used as object type, array element type,
  or structural member type
- pointer declarations to incomplete named structural types are allowed
- object declarations of incomplete named structural types are invalid until the
  type is completed
- each named structural type may transition from incomplete to complete at most
  once per module; incompatible redefinition is invalid
- completion must preserve type kind (`struct` cannot be completed as `union`,
  and vice versa)
- fixed-size array element type must be complete at declaration time
- VLA form `[* T]` is valid only for automatic storage within function bodies
- VLA form `[* T]` is invalid for file-scope/global declarations
- flexible array member form (`[* T]` as struct member) is valid only for the
  final struct member and only when at least one other named member exists

##### 3.10 Lowering Notes (Target Independent)

Charon may emit incomplete structural declarations when required for forward
reference fidelity. Verifier/type-resolution passes then validate completion
before any storage-bearing use of the type. Since TIIR pointers are opaque,
pointer-typed references can remain valid across the incomplete->complete
transition without pointer type rewriting.

##### 3.10 Test Coverage Status

Planned

- positive: forward struct declaration followed by one compatible completion
- positive: pointer symbol to incomplete struct type before completion
- positive: object declaration after struct completion
- positive: local VLA declaration in function body
- positive: valid trailing flexible array member after at least one named field
- negative: object declaration of incomplete struct type
- negative: second incompatible completion of same type name
- negative: kind mismatch completion (`struct` declared, `union` defined)
- negative: array with incomplete element type
- negative: VLA declaration at file scope
- negative: flexible array member as only named field or non-final member

#### 3.11 Type Compatibility and Composite Type Rules

##### 3.11 Feature

C99 compatibility/composite-type intent is preserved in TIIR, but evaluated over
TIIR's explicit type surface (fixed-width scalars, opaque `ptr`, named
structural types, fixed arrays, runtime-sized arrays in restricted contexts,
and explicit function signatures).

TIIR compatibility is used for:

- validating repeated declarations of the same symbol
- selecting effective type information when declaration + definition pairs are
  merged
- verifying call signatures and operand type matching

Composite type in TIIR v1 is the effective merged type chosen from compatible
declarations. Because TIIR is explicit, most composites are identity merges
(types already equal after charon lowering).

##### 3.11 Representative C Snippet

```c
struct S;
struct S { int x; };

int f(int);
int f(int a) { return a; }

extern int g;
int g;
```

##### 3.11 TIIR Canonical Form

```tiir
type @S = struct;
type @S = struct { %x: i32 };

symbol @f : (i32) -> i32;
symbol @f : (%a: i32) -> i32 [!linkage: external]:
  ret %a

symbol @g : i32 [!linkage: external];
symbol @g : i32;
```

##### 3.11 Grammar Productions Required

- no new syntax forms are required beyond existing type/declaration grammar
- composite-type behavior is a semantic merge rule over repeated declarations
- named structural declaration+completion forms from 3.10 participate in
  compatibility checks

##### 3.11 In-Memory Nodes Required

- per-symbol redeclaration chain with resolved effective type
- type-compatibility checker over TIIR type graph
- composite-type resolver producing a canonical effective type for each symbol
- diagnostic payload for first-mismatch location and mismatch category

##### 3.11 Semantic Validation Rules

- scalar compatibility:
  - integer types must match exactly in signedness and width (`i32` != `u32`)
  - floating types must match exactly (`f32` != `f64`)
  - `i1` is only compatible with `i1`
- pointer compatibility:
  - `ptr` is compatible with `ptr` (opaque pointer model)
- array compatibility:
  - fixed arrays are compatible iff element types are compatible and lengths
    are equal
  - runtime-sized arrays `[* T]` are compatible iff element types are
    compatible and both sides are runtime-sized form
  - fixed array and runtime-sized array forms are incompatible
- function compatibility:
  - return types must be compatible
  - parameter counts must match
  - corresponding parameter types must be compatible
  - variadic/non-variadic function attributes (when enabled) must match
- structural compatibility:
  - named structural types are compatible by resolved type identity after
    completion checks
  - completion must preserve kind (`struct` vs `union`)
  - incompatible completion/redefinition is invalid
- qualifier compatibility:
  - redeclared entities must have compatible qualifier sets under
    `!qualifiers`
- declaration composite selection:
  - if declarations are compatible, effective symbol type is their composite
    (normally identical in TIIR v1)
  - if declarations are incompatible, declaration set is invalid

##### 3.11 Lowering Notes (Target Independent)

Charon should emit fully normalized explicit types, minimizing ambiguous merge
cases. TIIR composite resolution therefore primarily acts as a verifier safety
pass for repeated declarations and forward-completion flows. Any C99 implicit
compatibility subtleties that survive frontend lowering must be made explicit
before TIIR emission.

##### 3.11 Test Coverage Status

Planned

- positive: compatible prototype + definition merge for function symbol
- positive: compatible extern declaration + tentative object definition merge
- positive: forward struct declaration followed by compatible completion
- positive: fixed array redeclarations with equal length and element type
- positive: runtime-sized array redeclarations with compatible element type in
  allowed contexts
- negative: integer signedness mismatch across redeclarations (`i32` vs `u32`)
- negative: float width mismatch across redeclarations (`f32` vs `f64`)
- negative: fixed-array length mismatch across redeclarations
- negative: fixed-array vs runtime-sized-array mismatch
- negative: function parameter count mismatch across redeclarations
- negative: function parameter type mismatch across redeclarations
- negative: return type mismatch across redeclarations
- negative: incompatible struct/union completion or redefinition
- negative: qualifier incompatibility across redeclarations

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

##### 4.10 Feature

C99 flexible array members are supported for struct declarations in TIIR using
the runtime-sized array form [* T] as the final member.

Canonical TIIR struct form:

- type @Packet = struct { %len: u32, %data: [* u8] };

Flexible array members are declaration-only shape elements. They describe
trailing variable-size storage for containing objects.

##### 4.10 Representative C Snippet

```c
struct Packet {
  unsigned len;
  unsigned char data[];
};
```

##### 4.10 TIIR Canonical Form

```tiir
type @Packet = struct { %len: u32, %data: [* u8] };

symbol @process_packet : (%p: ptr) -> void [!linkage: external]:
%process_packet_bb0:
  gep @Packet %len_ptr, %p, %len
  load u32 %len, %len_ptr
  gep @Packet %data_ptr, %p, %data
  ret
```

##### 4.10 Grammar Productions Required

- struct member declaration allows flexible array member form in trailing
  position:
  - struct_elem = ["%" id ":"] type
  - flexible_struct_elem = ["%" id ":"] "[" "*" type "]"
- flexible_struct_elem is valid only as the final member in struct { ... }
- union declarations do not allow flexible array members in v1

##### 4.10 In-Memory Nodes Required

- struct member descriptor flag: IS_FLEXIBLE_ARRAY
- flexible member element type storage
- verifier support for final-member and minimum-named-member constraints

##### 4.10 Semantic Validation Rules

- flexible array member is permitted only in struct types
- flexible array member must be the last member in the struct
- struct containing a flexible array member must have at least one other named
  member before it
- flexible array member cannot be the only named member
- flexible array member declarations do not carry an explicit constant bound
- object-size reasoning for such structs excludes trailing flexible storage
  unless an allocation site provides dynamic extent information

##### 4.10 Lowering Notes (Target Independent)

Charon lowers C flexible members to [* T] trailing struct members. Access to
the flexible member base uses standard gep member selection (by index or tagged
member name), and element addressing beyond the base is computed with
additional gep operations. This requires no gep syntax change beyond the already
adopted selector rules.

##### 4.10 Test Coverage Status

Planned

- positive: valid struct with fixed header fields and trailing flexible member
- positive: gep resolution to tagged flexible member base
- negative: flexible member in non-final position
- negative: flexible member as the only named member
- negative: flexible member declared in union

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

## TIIR Specification

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
