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
- VLA lowering requires `alloca [* T] %dst, %size` plus matching scope-exit
  `dealloca`.
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
- Typed metadata contracts are supported via:
  - `!type: <type_expr>` for type-structured constraints
  - `!literal: <literal_expr>` for literal-structured constraints

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
- Declaration metadata for constrained keys must be coherent with scope and
  declaration kind.
- Unknown metadata keys are allowed only as forward-compatible annotations, not
  replacements for constrained core semantics.
- `!type` metadata values must parse as valid TIIR type expressions.
- `!literal` metadata values must parse as valid TIIR literal expressions.
- C init-declarator lists must be flattened to one TIIR symbol declaration per
  declared symbol.
- Each flattened declarator initializer must be validated independently against
  its declared type.
- Fixed-size array initializers must not exceed declared bound.
- Runtime-sized arrays `[* T]` cannot use fixed aggregate literal initializers.
- VLA stack allocation must use explicit runtime size form
  `alloca [* T] %dst, %size`.
- VLA size operands must be integer-typed and positive when validated.
- Incomplete object declarations cannot carry initializers.
- C pointer declarator depth and qualifier placement must be normalized to TIIR
  opaque `ptr` plus declaration metadata.
- Pointer qualifiers from declarators must be preserved in `!qualifiers` where
  semantically relevant.
- Pointer declarator normalization must not alter declared symbol category
  (object vs function) or linkage/storage semantics.

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
- Any pointer declarator form that resolves to pointer object/function-pointer
  data must lower to TIIR `ptr` type.
- Pointer-chain depth from C declarators is not represented as nested pointer
  types in TIIR v1; frontend normalization is authoritative.
- Parameter array declarators must decay to pointer parameters in TIIR function
  signatures.
- Parameter-array qualifier intent must be preserved on lowered pointer
  parameter metadata.
- Parameter-only array attributes (for example C99 parameter `static` bound
  intent) are invalid outside parameter declarator context.
- If a pointer declaration carries `!type`, pointer use sites (`load`, `store`,
  `gep`) must be compatible with the declared `!type` contract.
- For array-parameter-derived pointer constraints, `!type: [N T]` encodes a
  minimum accessible contiguous-element contract of `N` elements of `T`.
- Array element type must be complete and non-`void`.
- Function parameter raw array/function forms must be decayed before TIIR emission.
- Direct call argument arity/types must match function signature.
- Fixed-size array length must be positive compile-time integer.
- `[* T]` is valid only in function-scope automatic storage or flexible-array-member context.
- `[* T]` is invalid for file-scope/global symbols.
- `[* T]` as struct member is valid only for trailing flexible member with at least one prior named member.
- `alloca [* T]` requires an explicit runtime size operand.
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
- positive: extern + const declaration normalized to linkage + qualifier metadata.
- positive: static + volatile declaration normalized to internal linkage + qualifier metadata.
- positive: mixed initialized/uninitialized C init-declarator list flattened to separate TIIR symbol entries.
- positive: extern declarator list flattened with inherited linkage metadata on each symbol.
- positive: fixed array declarator initialized with matching-bound aggregate literal.
- positive: single-level pointer declarator lowered to TIIR `ptr`.
- positive: multi-level pointer declarator lowered to TIIR `ptr` with preserved
  qualifier metadata.
- positive: function-pointer declarator lowered to TIIR pointer-typed symbol.
- positive: fixed-size object array declarator lowered to `[N T]`.
- positive: parameter array declarator decays to pointer parameter.
- positive: parameter-array qualifiers preserved in lowered pointer metadata.
- positive: parameter static-bound intent preserved as pointer `!type` array
  contract metadata.
- positive: pointer `!type` contract validates compatible `gep` base-type usage.
- negative: multiple object definitions in one translation unit.
- negative: incompatible redeclaration type mismatch.
- negative: conflicting linkage redeclarations.
- negative: multiple external definitions across modules.
- negative: incompatible storage-class redeclaration.
- negative: incompatible linkage/qualifier metadata across redeclarations.
- negative: invalid `restrict` qualifier on non-pointer declaration.
- negative: initializer type incompatible with declared type.
- negative: fixed array literal initializer exceeds declared bound.
- negative: initializer on incomplete object type.
- negative: fixed literal initializer on runtime-sized array declaration.
- negative: pointer qualifier conflict across redeclarations.
- negative: parameter-only array attributes used outside parameter context.
- negative: invalid `!type` metadata value (not parseable as type expression).
- negative: invalid `!literal` metadata value (not parseable as literal expression).
- negative: pointer use (`load`/`store`/`gep`) incompatible with pointer `!type`
  contract.
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
- positive: pointer declarator normalization keeps call/operand type checks valid.
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
- positive: local VLA lowered to `alloca [* T] %dst, %size` + scope-exit
  `dealloca`.
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
- negative: invalid declarator normalization causing object/function kind mismatch.
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
- `alloca type %dst` for fixed-extent automatic stack allocation
- `alloca [* T] %dst, %size` for VLA stack allocation with runtime extent
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
- if pointer operands carry `!type` contracts, `gep` base type and
  load/store element types must be contract-compatible
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

##### 4.1 Feature

C99 declaration specifiers (storage class, base type, qualifiers, and
struct/union/enum tags) are normalized by charon into TIIR declarations using:

- explicit TIIR type surface (`iN`, `uN`, `f32`, `f64`, `i1`, `ptr`, arrays,
  function signatures, named structural types)
- metadata tags for non-type declaration attributes (`!linkage`, `!storage`,
  `!qualifiers`, and optional extension/profile metadata)

In TIIR, declaration-specifier ordering from C source does not matter; the
normalized declaration payload is canonicalized by type + metadata.

##### 4.1 Representative C Snippet

```c
extern const int g;
static volatile int reg;
typedef struct Node Node;

struct Node {
  int value;
  struct Node *next;
};
```

##### 4.1 TIIR Canonical Form

```tiir
symbol @g : i32 [!linkage: external, !qualifiers: const];

symbol @reg : i32 [!linkage: internal, !qualifiers: volatile];

type @Node = struct;
type @Node = struct { %value: i32, %next: ptr };
```

##### 4.1 Grammar Productions Required

- declaration statement forms:
  - symbol declaration: `symbol @id : type [tag_use] ;`
  - symbol definition: `symbol @id : type [= initializer] [tag_use] ;`
  - type declaration/definition: `type @id = type_expr ;`
- `tag_use` must support declaration-level metadata list syntax
  (`[!key: value, ...]`)
- accepted declaration-specifier metadata keys for v1:
  - `!linkage`
  - `!storage`
  - `!qualifiers`

##### 4.1 In-Memory Nodes Required

- declaration node with:
  - declared symbol/type id
  - normalized TIIR type expression
  - declaration metadata map
  - declaration-vs-definition state
- redeclaration chain linkage for symbols and named types

##### 4.1 Semantic Validation Rules

- declaration metadata keys must be recognized for constrained keys in v1;
  unknown keys are allowed only as forward-compatible metadata, not as
  replacements for core semantics; if strict conformance is requested,
  unknown keys produce a warning diagnostic
- linkage/storage/qualifier metadata combinations must be coherent with scope:
  - `!linkage: internal` is valid for file-scope symbols
  - `!linkage: none` is used for local/block-scoped declarations
  - `!storage: automatic` applies to block-scope automatic declarations
- `!qualifiers` atoms must be from `{const, volatile, restrict}`
- `restrict` is valid only for pointer-typed declarations
- repeated declarations of same symbol/type must be compatible by type and
  constrained metadata semantics
- declaration specifier conflicts (e.g., incompatible type or incompatible
  linkage/qualifier set across redeclarations) are invalid

##### 4.1 Lowering Notes (Target Independent)

Charon performs C declaration-specifier parsing and conflict resolution before
TIIR emission, then emits canonical TIIR declarations with explicit type
surfaces plus metadata. This keeps TIIR declaration semantics order-insensitive
and straightforward to validate.

##### 4.1 Test Coverage Status

Planned

- positive: extern + const declaration lowered to `!linkage` + `!qualifiers`
- positive: static + volatile declaration lowered to internal linkage and
  volatile qualifier
- positive: forward named structural declaration then compatible completion
- negative: incompatible redeclaration type under same symbol
- negative: invalid `restrict` on non-pointer declaration
- negative: incompatible linkage/qualifier metadata across redeclarations

#### 4.2 Init Declarator Lists

##### 4.2 Feature

C99 init-declarator lists allow multiple declarators under one declaration
specifier sequence (for example `int a = 1, b, c = 3;`). TIIR normalizes this
surface form into one declaration/definition entry per symbol.

Each emitted TIIR symbol declaration carries:

- the fully resolved declared type
- declaration metadata inherited from declaration specifiers
- optional initializer payload for that specific declarator

TIIR does not require or preserve comma-separated declarator-list syntax.

##### 4.2 Representative C Snippet

```c
int a = 1, b, c = 3;
extern int x, y;
int arr[3] = {1, 2, 3};
```

##### 4.2 TIIR Canonical Form

```tiir
symbol @a : i32 = 1;
symbol @b : i32;
symbol @c : i32 = 3;

symbol @x : i32 [!linkage: external];
symbol @y : i32 [!linkage: external];

symbol @arr : [3 i32] = [1, 2, 3];
```

##### 4.2 Grammar Productions Required

- TIIR declaration grammar keeps one-symbol-per-declaration forms:
  - `symbol @id : type [= initializer] [tag_use] ;`
- C init-declarator-list parsing is frontend-only and must be normalized before
  TIIR emission
- initializer forms used in v1 include scalar literals and aggregate literals
  already defined elsewhere in this spec

##### 4.2 In-Memory Nodes Required

- symbol declaration/definition nodes with optional initializer payload
- initializer payload nodes for scalar and aggregate forms
- source-origin linkage (optional) from flattened TIIR declarations back to
  source declaration group for diagnostics

##### 4.2 Semantic Validation Rules

- each TIIR symbol declaration/definition entry is validated independently after
  frontend flattening
- declarator initializer type must be compatible with declared type
- fixed-size array literal initializers must not exceed declared bound
- runtime-sized arrays `[* T]` do not accept fixed literal initializers
- object declarations of incomplete types cannot carry initializers
- redeclaration/definition state rules from section 1.2 still apply after list
  flattening

##### 4.2 Lowering Notes (Target Independent)

Charon flattens C init-declarator lists into independent TIIR symbol entries
while preserving declaration-specifier-derived metadata on each symbol. This
eliminates C declarator-list parsing complexity from TIIR and keeps validation
local to single declarations.

##### 4.2 Test Coverage Status

Planned

- positive: mixed initialized/uninitialized declarator list flattened correctly
- positive: extern declarator list flattened with inherited linkage metadata
- positive: fixed array declarator initialized with matching bound literal
- negative: initializer type incompatible with declared type
- negative: fixed array literal initializer exceeds declared bound
- negative: initializer on incomplete object type
- negative: fixed literal initializer on runtime-sized array form
- negative: VLA alloca missing required runtime size operand.

#### 4.3 Pointer Declarators

##### 4.3 Feature

C99 pointer declarators can express multiple pointer levels and qualifier
placement through nested declarator syntax (`*`, `const`, `volatile`,
`restrict`, parenthesized declarator forms, and function-pointer/array-pointer
combinations).

TIIR v1 uses an opaque pointer model:

- any C pointer-declarator-derived type lowers to TIIR `ptr`
- pointer qualifier intent is preserved via declaration metadata (for example
  `!qualifiers`)
- pointer-chain depth is not represented as nested pointer type structure in the
  TIIR type surface

##### 4.3 Representative C Snippet

```c
int *p;
const int *restrict rp;
int (**fp)(int);
```

##### 4.3 TIIR Canonical Form

```tiir
symbol @p : ptr;
symbol @rp : ptr [!qualifiers: const, restrict];
symbol @fp : ptr;
```

##### 4.3 Grammar Productions Required

- no dedicated pointer-declarator syntax in TIIR text form beyond pointer type
  keyword `ptr`
- declaration grammar must allow pointer-qualified metadata through tag lists
  (`[!qualifiers: ...]`)
- function-pointer and pointer-to-array declarator complexity is frontend-only;
  normalized TIIR emits `ptr`

##### 4.3 In-Memory Nodes Required

- pointer type singleton (`ptr`)
- declaration metadata storage for pointer-related qualifiers
- optional frontend-origin annotation of original declarator shape for
  diagnostics/debugging

##### 4.3 Semantic Validation Rules

- any declaration normalized as pointer-derived must use TIIR `ptr`
- pointer qualifier atoms must satisfy existing qualifier rules
- pointer redeclarations must be type-compatible (`ptr` with compatible
  metadata)
- declarator normalization must preserve declared entity class:
  - object declarations remain objects
  - function declarations remain functions
  - function-pointer objects remain pointer-typed objects

##### 4.3 Lowering Notes (Target Independent)

Charon resolves C pointer declarator syntax and precedence, then emits TIIR
declarations with `ptr` plus qualifier metadata. Downstream typing remains
instruction-driven (`load T`, `store T`, `gep BaseType`) under opaque-pointer
semantics.

##### 4.3 Test Coverage Status

Planned

- positive: single-level pointer declarator lowered to TIIR `ptr`
- positive: qualifier-bearing pointer declarator lowered with qualifier metadata
- positive: function-pointer declarator lowered to pointer-typed object symbol
- negative: pointer qualifier incompatibility across redeclarations
- negative: normalization bug that changes object/function declaration class

#### 4.4 Array Declarators (Including Parameter Qualifiers)

##### 4.4 Feature

C99 array declarators describe fixed-size arrays, runtime-sized arrays (VLA
contexts), flexible array members, and parameter-array declarators with
qualifiers. TIIR normalizes these forms into explicit array types or pointer
parameters depending on declaration context.

Normalization rules:

- object/file-scope array declarator with constant bound -> `[N T]`
- runtime-sized automatic/VLA context -> `[* T]`
- struct trailing flexible member -> `[* T]` (subject to 4.10 constraints)
- function parameter array declarator -> parameter type `ptr` with preserved
  qualifier intent in metadata

Parameter-array qualifiers (`const`, `volatile`, `restrict`, and optional C99
`static` bound intent) are declaration-context attributes, not first-class TIIR
array type operators.

Pointer parameters may additionally carry structured metadata contracts to
improve misuse detection. In particular, `!type: [N T]` on a pointer parameter
declares that the pointer must reference at least `N` contiguous elements of
type `T` for validated uses.

##### 4.4 Representative C Snippet

```c
int g[4];

void f(int a[static 8], const int b[restrict]) {
  (void)a;
  (void)b;
}

void vla(int n) {
  int t[n];
}
```

##### 4.4 TIIR Canonical Form

```tiir
symbol @g : [4 i32];

symbol @f : (
  %a: ptr [!type: [8 i32]],
  %b: ptr [!qualifiers: const, restrict]
) -> void [!linkage: external]:
  ret

symbol @vla : (%n: i32) -> void [!linkage: external]:
  alloca [* i32] %t, %n
  dealloca [* i32] %t
  ret
```

##### 4.4 Grammar Productions Required

- fixed array type form: `"[" integer_literal type "]"`
- runtime-sized array type form: `"[" "*" type "]"`
- VLA allocation instruction form: `alloca "[" "*" type "]" %dst, %size`
- declaration metadata support for parameter-array qualifiers/bounds when
  lowered to pointer parameters (for example `!qualifiers`, optional pointer
  contracts via `!type`)
- parameter array declarator syntax itself is frontend-only (charon), not a
  distinct TIIR surface form

##### 4.4 In-Memory Nodes Required

- array type node for fixed and runtime-sized forms
- declaration metadata fields for parameter-array-derived qualifiers
- optional pointer contract field for parsed `!type` metadata type expressions

##### 4.4 Semantic Validation Rules

- fixed array bound `N` must be a positive compile-time integer
- runtime-sized array form `[* T]` is restricted to allowed contexts (VLA and
  flexible-array-member contexts)
- VLA lowering must emit `alloca [* T] %dst, %size` with explicit runtime size
  operand
- parameter array declarators must decay to `ptr` in TIIR signature types
- parameter-array qualifiers must be preserved on lowered pointer parameter
  metadata
- parameter-only array declarator attributes (such as C99 `static` in parameter
  array syntax) are invalid outside parameter declarator context
- if emitted, pointer `!type` contract metadata must parse as a valid type
  expression
- for array-parameter `static N` intent, the lowered pointer contract must be
  representable as `!type: [N T]` with positive compile-time `N`

##### 4.4 Lowering Notes (Target Independent)

Charon resolves array declarator context before emission. Object arrays remain
explicit TIIR array types; parameter arrays become pointer parameters with
metadata contracts. VLA arrays are lowered to runtime-sized array stack
allocation with explicit `alloca [* T] %dst, %size` + `dealloca` lifetime
edges.

##### 4.4 Test Coverage Status

Planned

- positive: fixed-size object array declarator lowered to `[N T]`
- positive: parameter array declarator decays to pointer parameter
- positive: parameter array qualifier set preserved in pointer metadata
- positive: parameter `static` bound intent preserved as pointer `!type` array
  contract metadata
- positive: compatible `gep` base type accepted under pointer `!type` contract
- positive: VLA alloca includes explicit runtime size operand
- negative: non-positive fixed array bound
- negative: parameter-only array attributes used outside parameter context
- negative: invalid/non-parseable pointer `!type` contract metadata
- negative: `gep`/memory use incompatible with pointer `!type` contract
- negative: VLA alloca emitted without runtime size operand

#### 4.5 Function Declarators and Prototypes

##### 4.5 Feature

C99 function declarators and prototypes define callable symbol signatures,
including parameter types, return type, and declaration-vs-definition state.
TIIR represents callable declarations directly through function type surfaces and
symbol declarations/definitions.

Normalization rules:

- function declaration without body lowers to declaration-only symbol entry
- function declaration with body lowers to function definition entry
- function parameter names in prototypes are source-level only; TIIR function
  type compatibility is based on ordered parameter type list and return type
- a parameter declared as `void` in C lowers to an empty parameter list in TIIR
  function signature type

In TIIR text, function symbols use:

- declaration form: `symbol @id : (param_types) -> return_type;`
- definition form: `symbol @id : (named_params) -> return_type [tag_use]: ...`

##### 4.5 Representative C Snippet

```c
int sum(int a, int b);

int sum(int a, int b) {
  return a + b;
}

int noop(void);
```

##### 4.5 TIIR Canonical Form

```tiir
symbol @sum : (i32, i32) -> i32;

symbol @sum : (%a: i32, %b: i32) -> i32 [!linkage: external]:
%sum_bb0:
  add i32 %0, %a, %b
  ret %0

symbol @noop : () -> i32;
```

##### 4.5 Grammar Productions Required

- function type form:
  - `function_type = "(" [type {"," type}] ")" "->" type`
- function declaration form:
  - `symbol_decl = "symbol" global_symbol ":" function_type [tag_use] ";"`
- function definition header form:
  - `symbol_def = "symbol" global_symbol ":" "(" [param_decl {"," param_decl}] ")" "->" type [tag_use] ":"`
- parameter declaration form in definition headers:
  - `param_decl = local_symbol ":" type [tag_use]`
- body form and label/instruction sequence follow existing statement/block rules

##### 4.5 In-Memory Nodes Required

- function signature type node:
  - ordered parameter type list
  - return type
- function symbol node with declaration/definition state
- optional parameter symbol list for definition headers (names + types +
  metadata)
- optional function body payload (basic blocks + instructions)
- redeclaration chain for prototype/definition matching

##### 4.5 Semantic Validation Rules

- repeated function declarations must be type-compatible:
  - same parameter count
  - pairwise compatible parameter types
  - compatible return type
- a function definition must be compatible with prior declarations
- at most one function definition per symbol is allowed in a translation unit
- `ret` usage must match declared return type:
  - `ret` with no operand only for `-> void`
  - `ret` with one operand only for non-void return types
- direct `call` argument arity and operand types must match callee signature
- prototype parameter identifiers (if present in C source) do not create runtime
  symbols in TIIR declaration-only entries

##### 4.5 Lowering Notes (Target Independent)

Charon resolves full C declarator precedence for function forms (including
parenthesized declarators and pointer-adjacent syntax), then emits canonical
TIIR function signatures. Prototype-only declarations remain body-free symbol
entries; definitions emit named local parameters and block/instruction bodies.

##### 4.5 Test Coverage Status

Planned

- positive: prototype-only declaration lowered to declaration-only function symbol
- positive: prototype followed by compatible definition
- positive: `void` parameter list lowers to empty TIIR parameter list `()`
- positive: call site with matching arity/types accepted
- negative: function parameter count mismatch across redeclarations
- negative: function parameter type mismatch across redeclarations
- negative: return type mismatch across redeclarations
- negative: second function definition in one translation unit
- negative: call argument arity/type mismatch against declared callee signature
- negative: incompatible `ret` form for declared return type

#### 4.6 Abstract Declarators and Typedef Names

##### 4.6 Feature

C99 abstract declarators (declarators without an identifier) and `typedef`
names are source-surface constructs consumed by charon before TIIR emission.
TIIR keeps only normalized type expressions and optional named type aliases.

Normalization rules:

- abstract declarators in casts/`sizeof`/compound-literal contexts are resolved
  to concrete TIIR type expressions before emission
- pointer/array/function declarator precedence is frontend-resolved and emitted
  as TIIR canonical type forms (`ptr`, `[N T]`, `[* T]`, `(params) -> ret`)
- C `typedef` declarations do not produce runtime symbol entries
- when typedef-name preservation is desired, charon may emit `type @Alias =
  type_expr;` as an alias declaration in the named type table

##### 4.6 Representative C Snippet

```c
typedef int i32_t;
typedef struct Node Node;

struct Node {
  int value;
  Node *next;
};

int cast_demo(void *p) {
  return *(i32_t *)p;
}
```

##### 4.6 TIIR Canonical Form

```tiir
type @i32_t = i32;
type @Node = struct;
type @Node = struct { %value: i32, %next: ptr };

symbol @cast_demo : (%p: ptr) -> i32 [!linkage: external]:
%cast_demo_bb0:
  load i32 %0, %p
  ret %0
```

##### 4.6 Grammar Productions Required

- alias-capable type declaration form:
  - `type_decl = "type" global_symbol "=" type ";"`
- named structural forward/complete declarations remain as defined in 3.10 and
  4.7
- no TIIR production is required for C abstract declarator syntax; that parsing
  is frontend-only

##### 4.6 In-Memory Nodes Required

- named type alias entry: `(name, target_type_expr)`
- named type table that resolves aliases and structural names
- optional source-origin attachment for alias/typedef diagnostics
- no runtime symbol node for typedef declarations

##### 4.6 Semantic Validation Rules

- typedef declarations must not emit TIIR runtime symbols
- alias target type expression must be a valid TIIR type
- alias redefinition must be compatible with prior alias/type entry for the
  same name; incompatible redefinition is invalid
- alias names participate in named type table resolution and are distinct from
  function/local symbol scope
- abstract-declarator-derived types used in casts or operators must be fully
  normalized before TIIR emission

##### 4.6 Lowering Notes (Target Independent)

Charon should preserve typedef-name spellings only when useful for diagnostics
or readability. Type checking and instruction typing in TIIR always use the
resolved canonical type expression, not source typedef spelling.

##### 4.6 Test Coverage Status

Planned

- positive: scalar typedef lowered to `type @Alias = iN`
- positive: forward typedef to named struct followed by compatible completion
- positive: abstract declarator in cast context lowers to canonical TIIR type
- negative: incompatible typedef/alias redefinition
- negative: typedef incorrectly emitted as runtime `symbol`
- negative: invalid alias target type expression

#### 4.7 Struct and Union Declarations

##### 4.7 Feature

C99 struct/union declarations map to TIIR named structural type declarations.
TIIR supports both incomplete declarations and completed definitions.

Canonical forms:

- incomplete declaration: `type @T = struct;` or `type @U = union;`
- completed definition:
  - `type @T = struct { [%member: ]type, ... };`
  - `type @U = union { [%member: ]type, ... };`

Member tags are optional, use `%id` syntax, and are local to the declaring
struct/union type.

##### 4.7 Representative C Snippet

```c
struct Node;

struct Node {
  int value;
  struct Node *next;
};

union Bits {
  int i;
  float f;
};
```

##### 4.7 TIIR Canonical Form

```tiir
type @Node = struct;
type @Node = struct { %value: i32, %next: ptr };

type @Bits = union { %i: i32, %f: f32 };
```

##### 4.7 Grammar Productions Required

- incomplete named structural declarations:
  - `type_decl = "type" global_symbol "=" "struct" ";"`
  - `type_decl = "type" global_symbol "=" "union" ";"`
- completed named structural definitions:
  - `type_def = "type" global_symbol "=" "struct" "{" struct_elem_list "}" ";"`
  - `type_def = "type" global_symbol "=" "union" "{" struct_elem_list "}" ";"`
- structural member form:
  - `struct_elem = [local_symbol ":"] type`
- member tags are optional but, when present, must be `%`-prefixed local symbols

##### 4.7 In-Memory Nodes Required

- named structural type table entries with completion state
- structural payload node:
  - kind (`struct` or `union`)
  - ordered member list `(optional_tag, member_type)`
- per-structure member-tag index map for `%tag` lookup in `gep`
- compatibility checker for repeated declarations/completions

##### 4.7 Semantic Validation Rules

- named structural types may transition incomplete -> complete at most once per
  module
- completion kind must match declaration kind (`struct` cannot complete as
  `union`, and vice versa)
- object declarations requiring complete structural layout are invalid before
  completion
- member tags must be unique within one structural type
- `%tag` member selection in `gep` must resolve in the base structural type's
  member namespace
- incompatible structural redefinition (member count/order/type/tag mismatch)
  is invalid

##### 4.7 Lowering Notes (Target Independent)

Charon should emit forward structural declarations only when needed for source
fidelity or dependency ordering, then emit one compatible completion. Opaque
pointer semantics (`ptr`) remain independent of structural completion timing.

##### 4.7 Test Coverage Status

Planned

- positive: forward struct declaration followed by one compatible completion
- positive: union definition with multiple member types
- positive: member-tagged struct used by `gep ... , %member`
- negative: incompatible second completion of same structural type name
- negative: kind-mismatch completion (`struct` declared, `union` completed)
- negative: duplicate member tag in one structural definition
- negative: `gep` member tag not found in selected structural base type

#### 4.8 Enum Declarations

##### 4.8 Feature

C99 enum declarations define named integer constants in the ordinary identifier
namespace. TIIR preserves enum declaration shape for readability and debug
fidelity while runtime semantics remain integer-typed.

Canonical form:

- `type @EnumName = enum { @E0 = v0, @E1 = v1, ... };`

Enumerator names are emitted as global symbols and referenced as `@id` in
operands.

##### 4.8 Representative C Snippet

```c
enum Mode {
  MODE_A,
  MODE_B = 4,
  MODE_C
};

enum Mode pick(int v) {
  if (v == 0) return MODE_A;
  if (v == 1) return MODE_B;
  return MODE_C;
}
```

##### 4.8 TIIR Canonical Form

```tiir
type @Mode = enum { @MODE_A = 0, @MODE_B = 4, @MODE_C = 5 };

symbol @pick : (%v: i32) -> i32 [!linkage: external]:
%pick_bb0:
  eq i32 %0, %v, @MODE_A
  br %0, %pick_bb_a, %pick_bb1

%pick_bb1:
  eq i32 %1, %v, @MODE_B
  br %1, %pick_bb_b, %pick_bb_c

%pick_bb_a:
  ret @MODE_A

%pick_bb_b:
  ret @MODE_B

%pick_bb_c:
  ret @MODE_C
```

##### 4.8 Grammar Productions Required

- enum type definition form:
  - `type_def = "type" global_symbol "=" "enum" "{" enum_elem_list "}" ";"`
- enum element form:
  - `enum_elem = global_symbol ["=" integer_literal]`
- comma-separated enum element list with optional trailing comma
- enum operand references use ordinary global symbol operand form (`@id`)

##### 4.8 In-Memory Nodes Required

- enum type descriptor entry:
  - enum type name
  - ordered enumerator list `(name, resolved_value)`
  - selected underlying integer type
- global symbol table entries for each enumerator constant
- resolver map from enumerator name to constant value and symbol entry

##### 4.8 Semantic Validation Rules

- enumerator initializers must be compile-time integer constants
- implicit enumerator values increment prior resolved value by `+1`
- duplicate enumerator names within one enum are invalid
- duplicate enumerator names across enums in one translation unit are invalid
  (ordinary identifier namespace rule)
- resolved enumerator values must fit selected underlying integer type
- enum constants must be referenced with `@id` in TIIR operands
- TIIR instruction typing for enum values uses integer type compatibility rules

##### 4.8 Lowering Notes (Target Independent)

Charon should resolve all enumerator values before emission and select the
underlying integer type per target ABI policy. TIIR may keep enum type
descriptors and symbolic enumerator names, while optimization/codegen can fold
them to integer immediates as needed.

##### 4.8 Test Coverage Status

Planned

- positive: enum with implicit and explicit value sequence
- positive: enum constants used in compare/branch and return paths
- positive: enum declaration retained as named type descriptor
- negative: non-constant enumerator initializer
- negative: duplicate enumerator name in same translation unit
- negative: enumerator value out of range for selected underlying type
- negative: enum operand used without `@` prefix

#### 4.9 Bit-Fields

##### 4.9 Feature

C99 bit-fields are supported for struct/union member declarations as
width-constrained integer members. TIIR models bit-field declarations directly
in structural type definitions, while preserving target-dependent layout policy
as verifier/lowering metadata.

Canonical TIIR member form:

- named bit-field member: `%name: base_int_type : width`
- unnamed bit-field member: `base_int_type : width`

Examples:

- `%mode: u32 : 3`
- `u32 : 5`
- `u32 : 0` (zero-width separator, unnamed only)

##### 4.9 Representative C Snippet

```c
struct Flags {
  unsigned mode : 3;
  unsigned ready : 1;
  unsigned : 4;
  unsigned value : 8;
};
```

##### 4.9 TIIR Canonical Form

```tiir
type @Flags = struct {
  %mode: u32 : 3,
  %ready: u32 : 1,
  u32 : 4,
  %value: u32 : 8
};

symbol @read_mode : (%p: ptr) -> u32 [!linkage: external]:
%read_mode_bb0:
  gep @Flags %bf_ptr, %p, %mode
  load u32 %mode, %bf_ptr
  ret %mode
```

##### 4.9 Grammar Productions Required

- extend structural member grammar with bit-field declarator form:
  - `struct_elem = [local_symbol ":"] type`
  - `bitfield_elem = [local_symbol ":"] integer_type ":" integer_literal`
- structural element list accepts either `struct_elem` or `bitfield_elem`
- zero-width separator form is represented by `bitfield_elem` with width `0`
  and no member tag
- `bitfield_elem` is valid only in `struct`/`union` member contexts

##### 4.9 In-Memory Nodes Required

- structural member descriptor extension:
  - `is_bitfield` flag
  - `base_integer_type`
  - `bit_width`
  - optional `member_tag`
- layout-derived metadata (post-layout pass):
  - storage unit index/offset
  - bit offset within storage unit
- member-tag lookup table must include named bit-field entries for `gep`

##### 4.9 Semantic Validation Rules

- bit-field base type must be an integer type (`iN`, `uN`, or `i1`)
- bit-field width must be a compile-time non-negative integer
- for non-zero widths: `1 <= width <= bitwidth(base_integer_type)`
- zero-width bit-field is valid only for unnamed members
- named zero-width bit-fields are invalid
- duplicate member tags within one structural type are invalid, including
  bit-field members
- bit-field declarations are valid only inside struct/union type definitions
- `gep ... , %tag` is valid for named bit-field members and resolves by member
  tag namespace rules from section 1.6
- load/store type compatibility for bit-field lvalues must use the declared
  bit-field base integer type

##### 4.9 Lowering Notes (Target Independent)

Charon should preserve bit-field declarations in structural type descriptors,
then apply target layout policy during type-layout/verifier passes. Accesses to
bit-fields may be lowered to canonical mask/shift/update sequences over the
selected storage unit while preserving source-level member identity.

Zero-width unnamed bit-fields act as layout separators according to target ABI
policy and must not produce addressable named members.

##### 4.9 Test Coverage Status

Planned

- positive: struct with multiple named bit-fields of compatible widths
- positive: unnamed non-zero-width bit-field separator member
- positive: unnamed zero-width bit-field layout separator
- positive: `gep` member selection by named bit-field tag
- negative: bit-field width greater than base integer bit width
- negative: named zero-width bit-field
- negative: non-integer base type used for bit-field
- negative: duplicate member tag where one or both are bit-fields
- negative: bit-field declaration outside struct/union member context

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

##### 5.1 Feature

C99 scalar initializers assign compile-time constant values to object
declarations at module scope (file-scope static/extern objects) and block scope
(local automatic/static objects). TIIR represents scalar initializers as
constant literal expressions attached to symbol declarations.

Scalar initializer forms in TIIR v1:

- integer literal: `42`, `0x1f`, `0b1010`
- floating literal: `3.14f`, `2.0`, `1e-5`
- enumerator reference: `@ENUM_VALUE`
- boolean literal: `0`, `1` (treated as `i1` values)
- null pointer: `null` (opaque `ptr` type)
- character literal (lowered to integer): `65` (for `'A'`)

Initializers are type-checked against declared object type and must be
representable in the target type without overflow (for integer types) or loss of
precision (for floating types, subject to ABI policy).

##### 5.1 Representative C Snippet

```c
int count = 42;
float ratio = 3.14f;
int *ptr = NULL;
enum Color shade = RED;
const int flags = 0x1f;
```

##### 5.1 TIIR Canonical Form

```tiir
symbol @count : i32 = 42;
symbol @ratio : f32 = 3.14;
symbol @ptr : ptr = null;
symbol @shade : i32 = @RED;
symbol @flags : i32 = 0x1f [!qualifiers: const];
```

##### 5.1 Grammar Productions Required

- symbol declaration with scalar initializer:
  - `symbol_decl = "symbol" global_symbol ":" type "=" initializer [tag_use] ";"`
- scalar initializer forms:
  - integer literal: `integer_constant`
  - floating literal: `float_constant`
  - enumerator/global symbol reference: `@id`
  - null pointer: `null`
- initializer must parse and validate against declared type

##### 5.1 In-Memory Nodes Required

- symbol declaration node with optional initializer payload:
  - initializer kind (literal_kind)
  - literal value (int/float/enum-ref/null)
  - source location for diagnostics
- initializer type-compatibility validator
- compile-time constant expression evaluator

##### 5.1 Semantic Validation Rules

- initializer type must be compatible with declared object type
- integer initializers must be representable in declared integer type width and
  signedness; overflow is invalid for signed types, wrapping is defined for
  unsigned types
- floating initializers must be representable in declared float type; precision
  loss or underflow is a lowering decision per target policy
- enumerator references (`@id`) are valid only when `@id` resolves to a global
  enumerator constant; the enumerator's integer value is used for type checking
- null pointer initializer (`null`) is valid only for pointer-typed objects
- character literals in C source are lowered to integer values by charon before
  TIIR emission and must fit in the target integer type
- boolean/logical initializers in C (`true`, `false`) are lowered to `1` or `0`
  (`i1` literals) before TIIR emission
- zero is implicitly assigned to uninitialized file-scope and tentative objects
  at end of translation unit by verifier if not explicitly initialized

##### 5.1 Lowering Notes (Target Independent)

Charon evaluates C compile-time constant expressions, type-converts initializers
to the declared type, and emits TIIR scalar initializers with explicit literal
values. Implicit zero initialization of tentative definitions is verified
post-parsing when an object is not explicitly initialized.

##### 5.1 Test Coverage Status

Planned

- positive: integer object initialization with representable literal
- positive: floating-point object initialization
- positive: pointer object initialization with `null`
- positive: enumerator constant used as scalar initializer
- positive: qualified object with compatible scalar initializer
- positive: implicit zero initialization of uninitialized file-scope object
- negative: integer initializer out of range for declared type
- negative: float initializer on integer-typed object
- negative: integer initializer on float-typed object
- negative: null pointer on non-pointer-typed object
- negative: enumerator reference to undefined enumerator name
- negative: initializer on incomplete object type

#### 5.2 Aggregate Initialization

##### 5.2 Feature

C99 aggregate initializers assign compile-time values to array and struct/union
objects. TIIR represents aggregate initializers as ordered lists of element
initializers (recursively scalars or nested aggregates) matched structurally to
the declared aggregate type.

Aggregate initializer forms in TIIR v1:

- array literal: `[elem0, elem1, ...]` for fixed-size arrays `[N T]`
- string literal: `"hello"` (shorthand for character array with null terminator)
- struct literal: `{ field0: value0, field1: value1, ... }` with optional
  member-tag or positional field selection
- union literal: `{ field: value }` (only one member initialized)
- nested aggregates: arrays/structs within struct/array initializers

Partial initialization fills remaining elements/members with zero.
Fixed-size arrays require initializer element count ≤ array bound. Runtime-sized
arrays `[* T]` do not accept fixed initializers in TIIR v1.

##### 5.2 Representative C Snippet

```c
int arr[4] = {1, 2};
struct Point { int x; int y; } p = {10, 20};
struct Mixed { int id; float val; } m = {1, 3.14f};
const char msg[] = "hello";
```

##### 5.2 TIIR Canonical Form

```tiir
symbol @arr : [4 i32] = [1, 2, 0, 0];

type @Point = struct { %x: i32, %y: i32 };
symbol @p : @Point = { %x = 10, %y = 20 };

type @Mixed = struct { %id: i32, %val: f32 };
symbol @m : @Mixed = { %id = 1, %val = 3.14 };

symbol @msg : [6 i8] = "hello";
```

##### 5.2 Grammar Productions Required

- array literal form:
  - `array_init = "[" [init_elem {"," init_elem}] [","] "]"`
- string literal form:
  - `string_literal = '"' char_sequence '"'`
  - equivalent to array of `i8` with implicit null terminator
- struct literal form:
  - `struct_init = "{" [struct_field {"," struct_field}] [","] "}"`
  - `struct_field = ["%tag" "="] init_elem`
- union literal form:
  - `union_init = "{" struct_field "}"`
  - exactly one member initialized
- init_elem is recursively scalar or aggregate initializer

##### 5.2 In-Memory Nodes Required

- aggregate initializer node with ordered element list
- array initializer payload: `(elements: [initializer])`
- struct initializer payload:
  - field-tagged map: `(field_tag_or_index -> initializer)`
  - element ordering preserved from source
- string literal payload:
  - character sequence with implicit null terminator
  - represented as array of `i8` constants
- initializer element can be scalar or nested aggregate
- partial-initialization tracker (remainder zero-filled by verifier)

##### 5.2 Semantic Validation Rules

- array initializer element count must not exceed declared array bound for
  fixed-size arrays
- each array element initializer must be type-compatible with array element
  type
- runtime-sized array form `[* T]` is invalid as an initializer target in v1
- struct initializer field mappings must use valid member tags (if tagged) or
  valid positional indices
- each struct member initializer must be type-compatible with declared member
  type
- uninitialized struct members are zero-filled (implicit)
- union initializer must specify exactly one member
- uninitialized union members are unspecified (post-verifier concern)
- string literal is valid only for character array types (`[N i8]`, `[N u8]`)
- string literal length (including null terminator) must not exceed declared
  array bound
- nested aggregates must recursively satisfy all aggregate validation rules
- struct field selection by tag requires the struct type to be complete and
  the tag to resolve in the struct's member-tag namespace

##### 5.2 Lowering Notes (Target Independent)

Charon lowering ensures initializers are fully expanded to element/member level
before TIIR emission. String literals are converted to character arrays with
implicit null terminator. Partial initialization (fewer elements/members than
aggregate size) is represented explicitly in TIIR by zero-filling omitted
elements, making the full extent of memory initialization transparent.

##### 5.2 Test Coverage Status

Planned

- positive: fixed array initialization with fewer elements than bound
- positive: fixed array initialization with all elements specified
- positive: struct initialization with all members tagged
- positive: struct initialization with positional member selection
- positive: nested aggregate initialization (array of structs)
- positive: string literal initialization for character array
- positive: string literal with implicit null terminator validation
- positive: union initialization with single member
- negative: array initializer element count exceeds bound
- negative: array element type incompatible with initializer
- negative: struct field tag unresolved or invalid
- negative: struct member type incompatible with initializer
- negative: union initializer with multiple members
- negative: string literal on non-character-array type
- negative: string literal length exceeds array bound
- negative: initializer on runtime-sized array `[* T]`
- negative: nested aggregate initializer with invalid element type
- negative: initializer on incomplete struct type

#### 5.3 Designated Initializers

##### 5.3 Feature

C99 designated initializers allow explicit field/element selection in aggregate
initializers, overriding positional ordering. TIIR normalizes designated
initializer syntax from C source into explicit element/member assignments,
then validates that the resulting aggregate covers required fields and remains
type-compatible.

Designated forms in TIIR v1:

- array designator: `[index] = value` (C99 syntax `arr[i] = ...`)
- struct designator: `%tag = value` (C99 syntax `.field = ...`)
- designator combinations: multiple designations within one initializer

##### 5.3 Representative C Snippet

```c
int arr[5] = {[2] = 42, [0] = 10};
struct Point { int x; int y; } p = {.y = 20, .x = 10};
struct Config { int a; int b; int c; } cfg = {.b = 5};
```

##### 5.3 TIIR Canonical Form

```tiir
symbol @arr : [5 i32] = [10, 0, 42, 0, 0];

type @Point = struct { %x: i32, %y: i32 };
symbol @p : @Point = { %x = 10, %y = 20 };

type @Config = struct { %a: i32, %b: i32, %c: i32 };
symbol @cfg : @Config = { %a = 0, %b = 5, %c = 0 };
```

##### 5.3 Grammar Productions Required

- array designator in initializer:
  - `[integer_literal] "=" init_elem`
- struct designator in initializer:
  - `"%tag" "=" init_elem`
- mixed positional and designated initializers within one aggregate (C99 allows
  this; TIIR normalizes to full element list)
- designator precedence: designated values override positional fill

##### 5.3 In-Memory Nodes Required

- aggregate initializer with designator metadata:
  - per-element/member: `(index_or_tag, initializer, is_designated)`
  - full canonical form after designation resolution
- designated initializer resolver that:
  - maps each designator to element/member index
  - fills gaps with zero/uninitialized
  - detects conflicts (same element designated twice)

##### 5.3 Semantic Validation Rules

- array designator index must be within range `[0, array_bound - 1]`
- struct designator tag must resolve in the struct's member-tag namespace
- designator and positional initializers may be mixed; positional elements
  fill gaps until next designator
- duplicate designations of the same element/member are invalid
- designated initializers do not require all members to be specified; unspecified
  members are zero-filled
- after designator resolution, resulting initializer must satisfy all aggregate
  type compatibility rules from 5.2

##### 5.3 Lowering Notes (Target Independent)

Charon frontend parses C designator syntax and produces a normalized canonical
initializer form in TIIR, expanding all designations to explicit element/member
positions and zero-filling gaps. This makes the full initialization extent
transparent and eliminates parser ambiguity.

##### 5.3 Test Coverage Status

Planned

- positive: array designator with index in range
- positive: struct designator with valid member tag
- positive: mixed positional and designated initializers
- positive: designated initializer with gaps filled by zero
- positive: multiple designated initializers in same aggregate
- negative: array designator index out of range
- negative: struct designator tag unresolved
- negative: duplicate designation of same element
- negative: designator with type-incompatible value

#### 5.4 Nested Designators

##### 5.4 Feature

C99 nested designators allow hierarchical specification of initializers for
deeply nested aggregates (arrays of structs, structs containing arrays, etc.).
TIIR normalizes nested designators to a flat canonical form by recursively
resolving each designator level and applying type-driven descent.

Nested designator forms in C:

- `arr[i].field` — array element followed by struct member
- `s.arr[j]` — struct member that is an array, indexed
- `s.inner.field` — nested struct member access
- `arr[i][j]` — multi-dimensional array indexing

TIIR flattens these to explicit intermediate values and final element values.

##### 5.4 Representative C Snippet

```c
struct Inner { int val; };
struct Outer { int id; struct Inner inner; } obj = {.inner.val = 42};
int matrix[3][3] = {[1][2] = 7};
```

##### 5.4 TIIR Canonical Form

```tiir
type @Inner = struct { %val: i32 };
type @Outer = struct { %id: i32, %inner: @Inner };
symbol @obj : @Outer = {
  %id = 0,
  %inner = { %val = 42 }
};

symbol @matrix : [3 [3 i32]] = [
  [0, 0, 0],
  [0, 0, 7],
  [0, 0, 0]
];
```

##### 5.4 Grammar Productions Required

- nested designator forms:
  - `designator = designator_elem {designator_elem}`
  - `designator_elem = "[" integer_literal "]" | "%tag"`
- charon parser resolves declarator context and normalizes nested designators
  into TIIR; no distinct nested-designator syntax is required in TIIR text form
  beyond the flat canonical initializer

##### 5.4 In-Memory Nodes Required

- nested initializer tree structure:
  - recursively defined aggregates with element/member initializers
  - leaf initializers are scalars or complete aggregate forms
- designator path resolution:
  - type-driven descent through nested aggregate types
  - conflict detection for overlapping or invalid paths

##### 5.4 Semantic Validation Rules

- each designator element must be valid for its aggregate level:
  - array designator on array type with index in bounds
  - struct designator on struct type with valid member tag
- designator path must resolve through complete types at each level
- nested designator cannot skip intermediate aggregates; full path must be
  specified
- resulting nested initializer must satisfy all type compatibility rules
  recursively

##### 5.4 Lowering Notes (Target Independent)

Charon performs full designator path resolution before TIIR emission, producing
a nested initializer tree that mirrors the target aggregate type structure.
This eliminates ambiguity and makes type-checking straightforward.

##### 5.4 Test Coverage Status

Planned

- positive: array-of-struct designator with nested member access
- positive: struct-with-array member designator with array indexing
- positive: multi-level struct nesting with designators
- positive: multi-dimensional array designator
- negative: designator path through incomplete type
- negative: invalid designator element at intermediate level
- negative: designator path mismatch with aggregate structure

#### 5.5 Zero Initialization Defaults

##### 5.5 Feature

C99 zero initialization fills uninitialized object storage with all-zero bit
patterns (for static/extern scope) or leaves the value indeterminate (for
automatic scope in C semantics). TIIR makes this explicit: file-scope objects
not otherwise initialized are zero-initialized by the verifier, and local
automatic objects without initializers are marked uninitialized (for later
analysis or explicit zero-init if required by policy).

##### 5.5 Representative C Snippet

```c
int g_uninitialized;              /* file-scope: implicitly zero */
static int s_uninitialized;       /* static: implicitly zero */
int f(void) {
  int local_uninit;               /* automatic: undefined in C */
  static int local_static = 0;    /* static within function: zero */
  return local_uninit;            /* undefined behavior */
}
```

##### 5.5 TIIR Canonical Form

```tiir
symbol @g_uninitialized : i32;
symbol @s_uninitialized : i32 [!linkage: internal];

symbol @f : () -> i32 [!linkage: external]:
%f_bb0:
  alloca i32 %local_static [!storage: static]
  ret %local_uninit
```

##### 5.5 Grammar Productions Required

- no distinct zero-initialization syntax in TIIR; zero-fill is a semantic
  property driven by storage class and declaration state
- verifier pass after parsing applies implicit zero initialization according to
  storage class rules

##### 5.5 In-Memory Nodes Required

- storage class annotation per symbol (`!storage: static`, `!storage: automatic`, etc.)
- declaration-state flag: `is_initialized`
- verifier metadata: zero-init policy per symbol (implicit for static scope, none
  for automatic)

##### 5.5 Semantic Validation Rules

- file-scope objects without initializers are tentative definitions; if not
  overridden by a complete definition with initializer, they are implicitly
  zero-initialized at module load time
- static-storage objects (including `static` within block scope) are implicitly
  zero-initialized if not explicitly initialized
- automatic-storage objects (local variables) without initializers have undefined
  values in C99 semantics; TIIR analysis may flag these for diagnostic purposes
- block-scope static objects are initialized once at program start; their
  initializer must be a compile-time constant expression

##### 5.5 Lowering Notes (Target Independent)

Charon frontend preserves zero-initialization intent via storage class and
initialization state. The TIIR verifier applies implicit zero-fill to static-
scope objects post-parsing. Automatic objects are left uninitialized (marked
with diagnostic metadata) to reflect C99 semantics.

##### 5.5 Test Coverage Status

Planned

- positive: file-scope object without initializer implicitly zero-initialized
- positive: static-storage object without initializer implicitly zero
- positive: static object within function scope initialized once
- positive: tentative definition without initializer later overridden by complete
  definition with initializer
- negative: use of uninitialized automatic variable (diagnostic only)

#### 5.6 String Literal Initialization for Char Arrays

##### 5.6 Feature

C99 string literals initialize character arrays directly. TIIR represents
string literals as `"..."` shorthand for arrays of `i8` or `u8` with implicit
null terminator. When assigned to a char array declaration, the string length
(including null terminator) must fit within the declared array bound.

##### 5.6 Representative C Snippet

```c
const char msg[] = "hello";
const char fixed[10] = "hi";
char *ptr = "literal";
```

##### 5.6 TIIR Canonical Form

```tiir
symbol @msg : [6 i8] = "hello";
symbol @fixed : [10 i8] = "hi";
symbol @ptr : ptr = @.str_literal_1
```

##### 5.6 Grammar Productions Required

- string literal form in initializer:
  - `string_literal = '"' [escape_sequence | char]* '"'`
- string literal in pointer assignment:
  - lowered to global character array symbol reference
- escape sequences (`\n`, `\t`, `\\`, `\"`, `\xHH`, `\OOO`, etc.) normalized by
  charon before TIIR emission

##### 5.6 In-Memory Nodes Required

- string literal storage:
  - character sequence (normalized escape sequences)
  - implicit null terminator (length includes `\0`)
  - array type `[N i8]` with `N = string_length + 1`
- string literal deduplication:
  - identical string literals may share single storage symbol (linker concern)
- string-to-pointer decay:
  - pointer assignments to string literals lower to symbol-ref operands

##### 5.6 Semantic Validation Rules

- string literal initializer for `[N T]` requires `T` to be `i8` or `u8`
  (character types)
- string literal length (including null terminator) must not exceed declared
  array bound
- string literal assigned to pointer is lowered to reference to character array
  symbol
- escape sequences must be valid (e.g., `\xHH` with valid hex digits)
- string literal width (character encoding) must match declared array element
  width

##### 5.6 Lowering Notes (Target Independent)

Charon lexer/frontend normalizes string literals and escape sequences, produces
canonical `i8` character sequences, and emits appropriate array symbol or
pointer reference. String deduplication is a backend/linker concern.

##### 5.6 Test Coverage Status

Planned

- positive: string literal for exact-fit char array
- positive: string literal shorter than array bound (partial init)
- positive: string literal assigned to pointer (decay to symbol reference)
- positive: escape sequences in string literal
- negative: string literal length exceeds array bound
- negative: string literal for non-character-array type
- negative: invalid escape sequence

#### 5.7 Compound Literals

##### 5.7 Feature

C99 compound literals create unnamed temporary aggregate values with explicit
type and initializer: `(type) { ... }` in expression context. TIIR represents
compound literals as anonymous static storage objects with initializers, then
referenced by pointer in expression operands (consistent with C99 semantics of
compound literal lvalues).

##### 5.7 Representative C Snippet

```c
struct Point { int x; int y; };
struct Point *p = &(struct Point) {10, 20};
int *arr_ptr = (int[3]) {1, 2, 3};
```

##### 5.7 TIIR Canonical Form

```tiir
type @Point = struct { %x: i32, %y: i32 };

symbol @.compound_1 : @Point = { %x = 10, %y = 20 } [!storage: static];
symbol @p : ptr = @.compound_1;

symbol @.compound_2 : [3 i32] = [1, 2, 3] [!storage: static];
symbol @arr_ptr : ptr = @.compound_2;
```

##### 5.7 Grammar Productions Required

- compound literal form:
  - `"(" type ")" "{" aggregate_initializer "}"`
- compound literals are frontend-only syntax; charon lowers them to anonymous
  static storage declarations and emits symbol references

##### 5.7 In-Memory Nodes Required

- anonymous symbol generation for each compound literal:
  - stable unique naming (e.g., `.compound_N`)
  - static storage class
  - initializer payload
- operand reference to anonymous symbol (pointer-typed)

##### 5.7 Semantic Validation Rules

- compound literal type must be a complete aggregate type (array, struct, or
  union)
- initializer must be compatible with declared compound literal type
- compound literal is an lvalue; address-of or pointer assignment is required
  to preserve the value
- block-scope compound literals have automatic storage lifetime within the
  enclosing block (TIIR may track scope extent for diagnostic purposes)
- file-scope compound literals have static storage lifetime

##### 5.7 Lowering Notes (Target Independent)

Charon frontend recognizes compound literal syntax, generates a unique storage
symbol with static scope and initializer, then replaces the literal expression
with a symbol reference (pointer-typed). Scope metadata is preserved for
block-scope compound literals to enable lifetime tracking if needed.

##### 5.7 Test Coverage Status

Planned

- positive: compound literal with aggregate initializer
- positive: address-of compound literal assigned to pointer
- positive: compound literal in block scope (diagnostic scope tracking)
- positive: compound literal with designated initializer
- negative: compound literal with incomplete type
- negative: compound literal initializer type mismatch
- negative: compound literal without aggregate initializer

### 6. Expressions and Conversions

#### 6.1 Primary Expressions

##### 6.1 Feature

C99 primary expressions are the atomic values in TIIR: literals, identifiers, and
parenthesized expressions. TIIR represents them as direct operands without
wrapper instructions, allowing them to be used directly in instruction operands.

Primary expression forms in TIIR v1:

- integer literal: `42`, `0x1f`, `0b1010`
- floating literal: `3.14`, `1e-5`
- character literal (lowered to integer): `65` (for `'A'`)
- enumerator constant: `@ENUM_NAME`
- identifier (symbol reference): `@global_sym` or `%local_sym`
- string literal: `@.str_1` (reference to global string symbol)
- null pointer: `null`
- parenthesized expression: evaluates to same type as inner expression

##### 6.1 Representative C Snippet

```c
int main(void) {
  int x = 42;
  float y = 3.14;
  int *p = NULL;
  enum Color c = RED;
  const char *msg = "hello";
  return x;
}
```

##### 6.1 TIIR Canonical Form

```tiir
symbol @main : () -> i32 [!linkage: external]:
%main_bb0:
  alloca i32 %x
  store i32 %x, 42
  alloca f32 %y
  store f32 %y, 3.14
  alloca ptr %p
  store ptr %p, null
  alloca i32 %c
  store i32 %c, @RED
  load i32 %result, %x
  ret %result
```

##### 6.1 Grammar Productions Required

- literal operand forms: `integer_const`, `float_const`, `null`
- identifier/symbol operand forms: `@id`, `%id`
- no special syntax required beyond existing operand productions

##### 6.1 In-Memory Nodes Required

- literal value nodes (int, float, null)
- symbol reference operand nodes (global or local)
- operand representation as immediate or symbol reference

##### 6.1 Semantic Validation Rules

- integer literals must be representable in operand-context integer type
- floating literals must be representable in operand-context float type
- symbol references must resolve to declared global symbols (`@`) or local
  symbols (`%`) in scope
- null pointer constant is valid only in pointer contexts

##### 6.1 Lowering Notes (Target Independent)

Charon emits primary expressions directly as operands; no instruction wrapping
is required.

##### 6.1 Test Coverage Status

Planned

- positive: integer literal in instruction operand
- positive: identifier reference in instruction operand
- positive: enumerator constant in operand
- positive: null pointer constant in pointer operand context
- negative: undefined symbol reference
- negative: literal out of representable range

#### 6.2 Postfix Operators

##### 6.2 Feature

C99 postfix operators include array subscripting `a[i]`, function calls `f(...)`,
member selection `.member` and `->member`, and post-increment/post-decrement.
TIIR lowers these to instruction sequences:

- array subscripting → `gep` + `load`
- member selection → `gep` + optional `load`
- function calls → `call`
- post-increment/post-decrement → `load`, `add`/`sub`, `store`

##### 6.2 Representative C Snippet

```c
int arr[3] = {1, 2, 3};
struct Point { int x; int y; } p = {10, 20};
int result = arr[1] + p.x;
int f(int);
int x = f(5);
```

##### 6.2 TIIR Canonical Form

```tiir
symbol @arr : [3 i32] = [1, 2, 3];
type @Point = struct { %x: i32, %y: i32 };
symbol @p : @Point = { %x = 10, %y = 20 };
symbol @f : (i32) -> i32;

symbol @result : () -> i32:
%result_bb0:
  gep [3 i32] %ptr_arr, @arr, 1
  load i32 %elem, %ptr_arr
  gep @Point %ptr_x, @p, %x
  load i32 %field_x, %ptr_x
  add i32 %sum, %elem, %field_x
  ret %sum
```

##### 6.2 Grammar Productions Required

- `gep` instruction: `gep type dest, src_ptr, selector`
- `load` instruction: `load type dest, ptr_operand`
- `call` instruction: `call (param_types) -> return_type dest, func, (args)`
- subscripting normalized to `gep` with integer index before TIIR emission
- member selection normalized to `gep` with integer index or `%tag` before TIIR
  emission

##### 6.2 In-Memory Nodes Required

- `gep` instruction node: base type, destination, source pointer, selector
- `load` instruction node: result type, destination, source pointer
- `call` instruction node: function type, destination, function reference, arguments

##### 6.2 Semantic Validation Rules

- array subscript must have compatible pointer or array type and integer index
- array/struct member access requires complete base aggregate type
- function call requires function pointer or function symbol
- function call arguments must match declared function parameter count and types
- result of array subscript is lvalue (addressable)
- result of member selection is lvalue if base is lvalue

##### 6.2 Lowering Notes (Target Independent)

Charon resolves C99 postfix operator precedence and lvalue/rvalue distinction,
then emits explicit `gep` + `load` sequences for array/member access and `call`
instructions for function invocation.

##### 6.2 Test Coverage Status

Planned

- positive: array subscript with integer index
- positive: struct member selection with tag
- positive: function call with matching argument types
- positive: chained postfix operators (e.g., array of struct)
- negative: array subscript on non-pointer/non-array type
- negative: function call with argument count mismatch
- negative: member selection on incomplete struct type

#### 6.3 Unary Operators

##### 6.3 Feature

C99 unary operators include address-of `&`, dereference `*`, negation `-`,
bitwise complement `~`, logical negation `!`, pre/post-increment/decrement,
sizeof, and _Alignof. TIIR lowers these to instruction sequences:

- address-of (`&`) → returns pointer to lvalue (no instruction; operand form)
- dereference (`*`) → `load` from pointer operand
- negation (`-`) → typed `neg` for integer and floating-point operands
- bitwise complement (`~`) → `xor -1, x`
- logical negation (`!`) → `eq i1 result, x, 0`
- sizeof → compile-time constant (no instruction)
- pre/post-increment → `load`, `add`/`sub`, `store`

##### 6.3 Representative C Snippet

```c
int x = 5;
int *p = &x;
int y = *p;
int neg = -x;
int comp = ~x;
int logical = !x;
int sz = sizeof(int);
```

##### 6.3 TIIR Canonical Form

```tiir
symbol @x : i32 = 5;
symbol @p : ptr;

symbol @test : () -> void:
%test_bb0:
  alloca i32 %local_x
  store i32 %local_x, 5
  store ptr %p, %local_x      /* address-of: directly reference address */
  load i32 %deref, %p
  load i32 %x_val, %local_x
  neg i32 %neg, %x_val
  xor i32 %comp, %x_val, -1
  eq i1 %logical, %x_val, 0
  ret
```

##### 6.3 Grammar Productions Required

- address-of operator: operand is lvalue, result is pointer (no instruction)
- dereference: `load type dest, ptr_operand`
- negation: `neg type dest, operand` (valid for integer and floating types)
- bitwise complement: `xor type dest, operand, -1`
- logical negation: `eq i1 dest, operand, 0`
- increment/decrement: `load`, `add`/`sub`, `store` sequence
- sizeof: compile-time constant value in operand

##### 6.3 In-Memory Nodes Required

- unary operator instruction nodes (neg, xor, eq, load for each form)
- lvalue-rvalue tracking for address-of and dereference
- compile-time constant evaluation for sizeof

##### 6.3 Semantic Validation Rules

- address-of requires an lvalue; result is pointer
- dereference requires pointer operand
- negation requires integer or floating operand
- bitwise complement requires integer operand
- logical negation converts operand to i1 condition
- sizeof operand must be a complete type
- increment/decrement requires integer or pointer operand and lvalue

##### 6.3 Lowering Notes (Target Independent)

Charon resolves unary operators to explicit instruction sequences. Address-of
and dereference are the primary pointer/lvalue mechanisms. Pre/post-increment
distinction is normalized to pre-increment semantics (explicit load-modify-store).

##### 6.3 Test Coverage Status

Planned

- positive: address-of lvalue producing pointer operand
- positive: dereference pointer operand via load
- positive: negation of integer
- positive: negation of float
- positive: bitwise complement of integer
- positive: logical negation producing i1 result
- positive: sizeof with complete type
- negative: address-of non-lvalue
- negative: dereference non-pointer type
- negative: negation of non-numeric type
- negative: sizeof incomplete type

#### 6.4 Cast Expressions

##### 6.4 Feature

C99 explicit casts convert values between types: `(type) expr`. TIIR lowering
produces type-conversion instructions for incompatible type pairs:

- integer → integer (different width/signedness): `trunc`, `zext`, `sext`
- integer → float: `sitofp` (signed), `uitofp` (unsigned)
- float → integer: `fptosi` (signed), `fptoui` (unsigned)
- float → float (different width): explicit conversion deferred
- pointer → integer: `ptrtoi` (size-compatible only)
- integer → pointer: `itop` (size-compatible only)
- pointer → pointer: implicit (no instruction in opaque model)

##### 6.4 Representative C Snippet

```c
int x = 42;
short s = (short) x;
float f = (float) x;
int i = (int) f;
int *p = (int *) 0x1000;
```

##### 6.4 TIIR Canonical Form

```tiir
symbol @test : () -> void:
%test_bb0:
  alloca i32 %x
  store i32 %x, 42
  load i32 %x_val, %x
  trunc i16 %s, %x_val       /* i32 -> i16 */
  sitofp f32 %f, %x_val      /* i32 -> f32 */
  fptosi i32 %i, %f
  itop ptr %p, 0x1000        /* i64 -> ptr */
  ret
```

##### 6.4 Grammar Productions Required

- cast syntax: `"(" type ")" expr`
- conversion instruction forms:
  - `trunc type_out dest, operand` (truncate to smaller integer)
  - `zext type_out dest, operand` (zero-extend to larger integer)
  - `sext type_out dest, operand` (sign-extend to larger integer)
  - `sitofp type_out dest, operand` (signed integer to float)
  - `uitofp type_out dest, operand` (unsigned integer to float)
  - `fptosi type_out dest, operand` (float to signed integer)
  - `fptoui type_out dest, operand` (float to unsigned integer)
  - `ptrtoi type_out dest, operand` (pointer to integer)
  - `itop type_out dest, operand` (integer to pointer)

##### 6.4 In-Memory Nodes Required

- cast/conversion instruction nodes: source type, destination type, operand
- type-compatibility checker for valid conversion paths

##### 6.4 Semantic Validation Rules

- source and target types must be compatible for conversion
- integer-to-integer conversions require valid width/signedness change
- integer-to-float and float-to-integer conversions are valid
- pointer-to-integer and integer-to-pointer conversions are valid only when
  source and target sizes are compatible
- incompatible pointer/integer cast sizes emit a diagnostic whose severity is
  warning or error according to requested diagnostic level/profile
- implicit conversions are applied per usual arithmetic conversion rules
- explicit casts bypass some implicit conversion restrictions

##### 6.4 Lowering Notes (Target Independent)

Charon determines the minimal conversion instruction sequence required. For
pointer/integer casts, charon (or verifier) must validate size compatibility
using target data model information and emit profile-configured diagnostics on
mismatch.

##### 6.4 Test Coverage Status

Planned

- positive: integer truncation cast
- positive: integer zero-extension cast
- positive: integer sign-extension cast
- positive: signed-integer to float conversion
- positive: float to signed-integer conversion
- positive: pointer-to-integer cast with compatible size
- positive: integer-to-pointer cast with compatible size
- negative: invalid cast between incompatible types (e.g., struct)
- negative: pointer-to-integer cast with incompatible size
- negative: integer-to-pointer cast with incompatible size
- negative: lossy cast without explicit conversion operator

#### 6.5 Multiplicative, Additive, and Shift Expressions

##### 6.5 Feature

C99 multiplicative, additive, and shift operators work on integer, floating,
and pointer operands (as context permits). TIIR lowers them to typed
instruction forms:

- multiplicative: `mul`, `div` (integer or float), `mod` (integer only)
- additive: `add`, `sub` (integer or float; pointer arithmetic allowed where
  valid)
- shift: `shl` (left shift), `lshr` (logical right shift), `ashr` (arithmetic
  right shift), valid only on integer types

##### 6.5 Representative C Snippet

```c
int a = 10, b = 3;
float fa = 10.0f, fb = 3.0f;
int sum = a + b;
int prod = a * b;
int quot = a / b;
int rem = a % b;
float fsum = fa + fb;
float fprod = fa * fb;
int shifted = a << 2;
```

##### 6.5 TIIR Canonical Form

```tiir
symbol @test : () -> i32:
%test_bb0:
  alloca i32 %a
  store i32 %a, 10
  alloca i32 %b
  store i32 %b, 3
  alloca f32 %fa
  store f32 %fa, 10.0
  alloca f32 %fb
  store f32 %fb, 3.0
  load i32 %a_val, %a
  load i32 %b_val, %b
  load f32 %fa_val, %fa
  load f32 %fb_val, %fb
  add i32 %sum, %a_val, %b_val
  mul i32 %prod, %a_val, %b_val
  div i32 %quot, %a_val, %b_val
  mod i32 %rem, %a_val, %b_val
  add f32 %fsum, %fa_val, %fb_val
  mul f32 %fprod, %fa_val, %fb_val
  shl i32 %shifted, %a_val, 2
  ret %sum
```

##### 6.5 Grammar Productions Required

- arithmetic instructions:
  - `add type dest, operand1, operand2`
  - `sub type dest, operand1, operand2`
  - `mul type dest, operand1, operand2`
  - `div type dest, operand1, operand2`
  - `mod type dest, operand1, operand2` (integer type only)
- shift instructions:
  - `shl type dest, operand, shift_amount`
  - `lshr type dest, operand, shift_amount`
  - `ashr type dest, operand, shift_amount`
- operand types must be consistent for the operation

##### 6.5 In-Memory Nodes Required

- binary arithmetic instruction nodes (add, sub, mul, div, mod)
- shift instruction nodes (shl, lshr, ashr)
- operand type compatibility checker
- division-by-zero detector (deferred)

##### 6.5 Semantic Validation Rules

- operands to `add`/`sub`/`mul`/`div` must be either integer-typed or
  floating-typed and must match instruction type
- pointer arithmetic is valid for `add` and `sub` with pointer and integer
  operands
- division and modulo by zero are invalid (verifier concern)
- `mod` operands must be integer-typed
- shift instructions are valid only on integer-typed operands; shift amount
  must be non-negative and less than bit width
- mixed-width operands require explicit conversion before operation
- implicit usual arithmetic conversions are applied pre-instruction emission

##### 6.5 Lowering Notes (Target Independent)

Charon applies usual arithmetic conversions to operands, then emits explicit
arithmetic instructions. Pointer arithmetic is handled per C99 semantics
(pointer scaled by element type).

##### 6.5 Test Coverage Status

Planned

- positive: integer addition
- positive: float addition
- positive: integer multiplication
- positive: float multiplication
- positive: integer division and modulo
- positive: shift operations with valid shift amounts
- positive: pointer addition with integer offset
- negative: shift on floating-point operand
- negative: shift amount out of range
- negative: division by zero
- negative: mixed-width operands without explicit conversion
- negative: pointer arithmetic on non-pointer operand

#### 6.6 Relational and Equality Expressions

##### 6.6 Feature

C99 relational (`<`, `<=`, `>`, `>=`) and equality (`==`, `!=`) operators
produce i1 (boolean) results. TIIR lowers them to comparison instructions:

- `eq` (equal)
- `ne` (not equal)
- `lt` (less than, signed)
- `le` (less than or equal, signed)
- `gt` (greater than, signed)
- `ge` (greater than or equal, signed)
- `ult` (unsigned less than)
- `ule` (unsigned less than or equal)
- `ugt` (unsigned greater than)
- `uge` (unsigned greater than or equal)

##### 6.6 Representative C Snippet

```c
int x = 10, y = 5;
int eq_result = (x == y);
int lt_result = (x < y);
int cond = (x > y) ? 1 : 0;
```

##### 6.6 TIIR Canonical Form

```tiir
symbol @test : () -> i32:
%test_bb0:
  alloca i32 %x
  store i32 %x, 10
  alloca i32 %y
  store i32 %y, 5
  load i32 %x_val, %x
  load i32 %y_val, %y
  eq i1 %eq_res, %x_val, %y_val
  lt i1 %lt_res, %x_val, %y_val
  gt i1 %gt_res, %x_val, %y_val
  ret %eq_res
```

##### 6.6 Grammar Productions Required

- comparison instructions:
  - `eq i1 dest, operand1, operand2`
  - `ne i1 dest, operand1, operand2`
  - `lt i1 dest, operand1, operand2` (signed)
  - `le i1 dest, operand1, operand2` (signed)
  - `gt i1 dest, operand1, operand2` (signed)
  - `ge i1 dest, operand1, operand2` (signed)
  - `ult i1 dest, operand1, operand2` (unsigned)
  - `ule i1 dest, operand1, operand2` (unsigned)
  - `ugt i1 dest, operand1, operand2` (unsigned)
  - `uge i1 dest, operand1, operand2` (unsigned)

##### 6.6 In-Memory Nodes Required

- comparison instruction nodes (eq, ne, lt, le, gt, ge, ult, ule, ugt, uge)
- result type always `i1`
- operand type compatibility checker

##### 6.6 Semantic Validation Rules

- operands to relational operators must be arithmetic types (integers, floats)
- or compatible pointer types
- result of comparison is always `i1`
- signed vs unsigned comparison is signedness-sensitive
- mixed-signedness comparisons require explicit conversion

##### 6.6 Lowering Notes (Target Independent)

Charon resolves operand signedness and chooses appropriate signed/unsigned
comparison instruction. Mixed-signedness operands are explicitly converted
before comparison.

##### 6.6 Test Coverage Status

Planned

- positive: equality comparison of integers
- positive: relational comparison of integers
- positive: pointer equality comparison
- positive: comparison result feeding into branch condition
- negative: comparison of non-arithmetic types
- negative: mixed-signedness comparison without conversion

#### 6.7 Bitwise and Logical Expressions

##### 6.7 Feature

C99 bitwise operators (`&`, `|`, `^`) work on integers; logical operators
(`&&`, `||`) short-circuit and produce i1 results. TIIR lowers bitwise
operators to instruction forms and represents logical operators as conditional
control flow:

- bitwise `and`, `or`, `xor` (integers only)
- bitwise `~` (complement) → `xor -1, x`
- logical `&&` → control flow short-circuit (result i1)
- logical `||` → control flow short-circuit (result i1)

##### 6.7 Representative C Snippet

```c
int a = 12, b = 10;
int bitwise_and = a & b;
int bitwise_or = a | b;
int bitwise_xor = a ^ b;
int logical_and = (a && b);
int logical_or = (a || b);
```

##### 6.7 TIIR Canonical Form

```tiir
symbol @test : () -> i32:
%test_bb0:
  alloca i32 %a
  store i32 %a, 12
  alloca i32 %b
  store i32 %b, 10
  load i32 %a_val, %a
  load i32 %b_val, %b
  and i32 %and_res, %a_val, %b_val
  or i32 %or_res, %a_val, %b_val
  xor i32 %xor_res, %a_val, %b_val
  ne i1 %a_cond, %a_val, 0       /* convert to i1 */
  ne i1 %b_cond, %b_val, 0
  and i1 %and_logical, %a_cond, %b_cond
  or i1 %or_logical, %a_cond, %b_cond
  ret %and_res
```

##### 6.7 Grammar Productions Required

- bitwise instructions:
  - `and type dest, operand1, operand2`
  - `or type dest, operand1, operand2`
  - `xor type dest, operand1, operand2`
- logical instructions (on i1):
  - `and i1 dest, cond1, cond2`
  - `or i1 dest, cond1, cond2`
- short-circuit semantics: logical `&&`/`||` use control flow (br instructions)

##### 6.7 In-Memory Nodes Required

- bitwise instruction nodes (and, or, xor)
- logical instruction nodes (and/or on i1)
- short-circuit control flow blocks for logical operators
- condition evaluation and branching

##### 6.7 Semantic Validation Rules

- bitwise operators require integer operands
- logical operators require operands convertible to i1 (via comparison or
  boolean context)
- short-circuit evaluation is mandatory for `&&` and `||`
- bitwise operators on `i1` are valid
- arithmetic operations on `i1` are invalid (except bitwise)

##### 6.7 Lowering Notes (Target Independent)

Charon lowering converts non-i1 operands to boolean conditions via comparison
before logical operations, and emits control-flow branches for short-circuit
semantics when logical operators are used in statement context.

##### 6.7 Test Coverage Status

Planned

- positive: bitwise and/or/xor of integers
- positive: logical and/or with short-circuit behavior
- positive: bitwise operations on `i1`
- negative: bitwise operation on non-integer type
- negative: arithmetic on `i1`
- negative: logical operator without short-circuit

#### 6.8 Conditional Operator

##### 6.8 Feature

C99 conditional expressions (`cond ? expr_true : expr_false`) select one of two
expressions based on a condition converted to boolean context. TIIR lowers the
conditional operator using the same control-flow strategy as logical
short-circuiting in 6.7: evaluate condition, branch to true/false blocks,
evaluate only the selected arm, then merge to a single result value.

Key properties:

- condition is converted to `i1` before branching
- exactly one arm is evaluated at runtime
- true and false arm result types must be compatible after usual conversions
- merged result has one effective type and can be used as an expression operand

##### 6.8 Representative C Snippet

```c
int x = 10;
int y = 5;
int max = (x > y) ? x : y;
float r = (x != 0) ? 1.5f : 2.5f;
```

##### 6.8 TIIR Canonical Form

```tiir
symbol @test : () -> i32:
%test_bb0:
  alloca i32 %x
  store i32 %x, 10
  alloca i32 %y
  store i32 %y, 5
  load i32 %x_val, %x
  load i32 %y_val, %y
  gt i1 %cond, %x_val, %y_val
  alloca i32 %max_tmp
  br %cond, %test_bb_true, %test_bb_false

%test_bb_true:
  store i32 %max_tmp, %x_val
  b %test_bb_merge

%test_bb_false:
  store i32 %max_tmp, %y_val
  b %test_bb_merge

%test_bb_merge:
  load i32 %max, %max_tmp
  ret %max
```

##### 6.8 Grammar Productions Required

- conditional expression surface form (frontend syntax):
  - `conditional_expr = logical_or_expr "?" expr ":" conditional_expr`
- TIIR lowering requires existing control-flow instruction forms:
  - `br cond, true_label, false_label`
  - `b label`
- merge-value lowering requires existing memory/value instructions:
  - `alloca type %tmp`
  - `store type %tmp, value`
  - `load type %dst, %tmp`

##### 6.8 In-Memory Nodes Required

- conditional-expression AST node (frontend) with:
  - condition expression
  - true-arm expression
  - false-arm expression
- lowered control-flow blocks for true/false/merge regions
- merged expression value representation (temporary storage or equivalent
  SSA-merge construct)
- type-resolution record for final conditional expression type

##### 6.8 Semantic Validation Rules

- condition expression must be convertible to `i1` boolean context
- true and false arm expressions must be type-compatible after usual
  arithmetic/pointer conversion rules
- only selected branch arm is evaluated at runtime
- merged result type is the composite/effective type derived from both arms
- if one arm is `void`, the other arm must also satisfy C99 conditional
  operator constraints for void conditional expressions
- pointer-typed conditional arms must satisfy pointer compatibility rules from
  6.13

##### 6.8 Lowering Notes (Target Independent)

Charon should lower `?:` using explicit control-flow blocks consistent with 6.7
short-circuit logic. Frontend type resolution determines a single effective
result type before emission. v1 may use temporary storage (`alloca`/`store`/
`load`) for merge value materialization; future profiles may replace this with a
dedicated SSA merge form.

##### 6.8 Test Coverage Status

Planned

- positive: integer conditional expression with relational condition
- positive: float conditional expression with arithmetic-compatible arms
- positive: pointer conditional expression with compatible pointer arms
- positive: conditional expression used as call argument
- negative: condition not convertible to boolean context
- negative: incompatible true/false arm types
- negative: invalid pointer arm compatibility
- negative: both arms evaluated (short-circuit violation)

#### 6.9 Assignment and Compound Assignment

##### 6.9 Feature

C99 assignment expressions write values to modifiable lvalues and produce an
expression result. TIIR models assignment through explicit memory operations and
typed arithmetic/bitwise instructions.

Assignment forms covered in v1:

- simple assignment: `=`
- arithmetic compound assignment: `+=`, `-=`, `*=`, `/=`, `%=`
- shift compound assignment: `<<=`, `>>=`
- bitwise compound assignment: `&=`, `|=`, `^=`

Lowering model:

- `lhs = rhs` -> compute `rhs`, convert if required, `store`, result is stored
  value
- `lhs op= rhs` -> `load lhs`, compute `lhs op rhs`, `store`, result is new
  stored value

##### 6.9 Representative C Snippet

```c
int x = 10;
int y = 3;
x = y;
x += 2;
x *= y;
x &= 7;
```

##### 6.9 TIIR Canonical Form

```tiir
symbol @test : () -> i32:
%test_bb0:
  alloca i32 %x
  store i32 %x, 10
  alloca i32 %y
  store i32 %y, 3

  /* x = y */
  load i32 %y_val, %y
  store i32 %x, %y_val

  /* x += 2 */
  load i32 %x0, %x
  add i32 %x1, %x0, 2
  store i32 %x, %x1

  /* x *= y */
  load i32 %x2, %x
  load i32 %y2, %y
  mul i32 %x3, %x2, %y2
  store i32 %x, %x3

  /* x &= 7 */
  load i32 %x4, %x
  and i32 %x5, %x4, 7
  store i32 %x, %x5

  ret %x5
```

##### 6.9 Grammar Productions Required

- assignment-expression frontend forms:
  - `assignment_expr = unary_expr assignment_op assignment_expr`
  - `assignment_op = "=" | "*=" | "/=" | "%=" | "+=" | "-=" | "<<=" | ">>=" | "&=" | "^=" | "|="`
- TIIR uses existing instruction forms for lowering:
  - `load type dest, ptr_operand`
  - `store type ptr_operand, value`
  - arithmetic/bitwise instructions from 6.5 and 6.7

##### 6.9 In-Memory Nodes Required

- assignment-expression AST node:
  - lhs expression (must resolve to modifiable lvalue)
  - assignment operator kind
  - rhs expression
- lowered instruction sequence nodes:
  - load/compute/store pipeline for compound assignment
- expression-result tracking for assignment expression value

##### 6.9 Semantic Validation Rules

- left-hand side must be a modifiable lvalue
- assignment to `const`-qualified objects is invalid
- assignment to array objects is invalid
- simple assignment requires rhs value to be convertible to lhs object type
- compound assignment requires:
  - lhs type and rhs type compatible with underlying operator
  - integer-only restrictions for `%`, shifts, and bitwise compound operators
  - shift compound assignments valid only for integer lhs/rhs
- pointer assignment requires pointer compatibility per 6.13
- assignment expression result type is the lhs type after required conversions

##### 6.9 Lowering Notes (Target Independent)

Charon lowers assignment to explicit side-effecting store operations. Compound
assignment preserves C99 evaluation semantics by loading lhs exactly once,
applying conversions/operator semantics, then storing once. The expression
result is the final stored value, represented either as the computed temporary
or a post-store load according to lowering strategy.

##### 6.9 Test Coverage Status

Planned

- positive: simple scalar assignment
- positive: arithmetic compound assignment (`+=`, `*=`) on integers
- positive: arithmetic compound assignment on floating types where allowed
- positive: bitwise compound assignment on integer type
- positive: shift compound assignment with valid integer operands
- positive: assignment expression used as subexpression value
- negative: assignment to non-modifiable lvalue (`const`)
- negative: assignment to array object
- negative: modulo/bitwise compound assignment on floating operand
- negative: shift compound assignment on non-integer type
- negative: incompatible rhs type without valid conversion

#### 6.10 Comma Operator

##### 6.10 Feature

C99 comma expressions evaluate the left operand, discard its value, then
evaluate and yield the right operand value. In TIIR v1 this is frontend-lowered
by charon into ordered evaluation using existing instruction sequencing within
the current basic block/control-flow graph.

No dedicated TIIR comma instruction is introduced.

##### 6.10 Lowering Notes (Target Independent)

Charon emits both operand evaluations in source order, preserves side effects
from the left expression, and forwards only the right expression value as the
result. This uses existing TIIR entities only (instruction order, branches,
temporaries, load/store/call/etc.).

##### 6.10 Grammar / In-Memory Impact

- frontend-only syntax handling in charon
- no new TIIR grammar productions required beyond existing expression lowering
  targets
- no new TIIR instruction node kinds required

##### 6.10 Validation Notes

- evaluation order must remain left-to-right for comma operands
- side effects in left operand must be preserved even when value is discarded

#### 6.11 Lvalue/Rvalue and Modifiable Lvalue Rules

##### 6.11 Feature

C99 lvalue/rvalue category checks and modifiable-lvalue constraints are enforced
by charon during semantic analysis and lowering. TIIR receives already-resolved
address/value usage mapped onto existing pointer and memory operations.

No distinct TIIR value-category metadata is required in v1 beyond existing type
and operand semantics.

##### 6.11 Lowering Notes (Target Independent)

Charon resolves whether each expression is used as an addressable object or
value, then lowers to existing TIIR forms:

- lvalue use -> address-producing forms (`alloca`, `gep`, symbol address)
- rvalue use -> value-producing forms (`load`, arithmetic/compare/cast results)
- assignment destination checks -> existing store legality validation

##### 6.11 Grammar / In-Memory Impact

- frontend-only value-category analysis
- no new TIIR instruction syntax required
- no new TIIR node families required; existing expression AST and instruction
  nodes are sufficient

##### 6.11 Validation Notes

- assignment/update operators require modifiable lvalues
- non-modifiable lvalue violations (for example `const` target, array target)
  must be diagnosed before TIIR emission

#### 6.12 Integer Promotions and Usual Arithmetic Conversions

##### 6.12 Feature

C99 integer promotions and usual arithmetic conversions are normalized by charon
before emitting TIIR arithmetic/comparison instructions. TIIR instructions
remain explicitly typed and do not perform implicit promotion at runtime.

No new TIIR conversion categories are introduced; existing conversion
instructions from 6.4 are reused.

##### 6.12 Lowering Notes (Target Independent)

Charon determines the common result type for each arithmetic/comparison
expression, inserts explicit conversions as needed (`trunc`, `zext`, `sext`,
`sitofp`, `uitofp`, `fptosi`, `fptoui`), then emits typed operations (`add`,
`mul`, comparisons, etc.) on converted operands.

##### 6.12 Grammar / In-Memory Impact

- frontend-only promotion/conversion policy
- no new TIIR grammar productions beyond existing cast/conversion instructions
- no new TIIR instruction kinds required

##### 6.12 Validation Notes

- operands reaching typed arithmetic/comparison instructions must already be
  conversion-compatible
- mixed-width/signedness cases must be normalized by inserted explicit
  conversions prior to emission

#### 6.13 Pointer Conversions and Null Pointer Constants

##### 6.13 Feature

C99 pointer compatibility, pointer conversions, and null pointer constant rules
are resolved by charon and lowered to existing TIIR pointer/value constructs.

TIIR v1 uses opaque `ptr` plus existing conversion instructions; no additional
pointer-type surface syntax is introduced.

##### 6.13 Lowering Notes (Target Independent)

Charon handles pointer conversion legality and null-constant normalization:

- null pointer constants lower to `null` in pointer contexts
- pointer-to-pointer compatibility checks are validated in frontend semantics
- pointer<->integer casts use existing `ptrtoi` / `itop` instructions from 6.4
  with size-compatibility diagnostics driven by target data model

##### 6.13 Grammar / In-Memory Impact

- frontend-only pointer compatibility resolution
- no new TIIR grammar productions beyond existing `ptr`, `null`, `ptrtoi`,
  and `itop` forms
- no new TIIR node kinds required

##### 6.13 Validation Notes

- null pointer constants are valid only in pointer conversion/assignment
  contexts
- invalid pointer compatibility or disallowed pointer/integer cast sizes must
  emit diagnostics according to configured severity profile

### 7. Statements and Control Flow

#### 7.1 Labeled Statements

##### 7.1 Feature

C99 labeled statements (`label: stmt`, `case`, `default`) are frontend control-
flow anchors. For ordinary labels, charon lowers directly into existing TIIR
basic-block labels.

No new TIIR entity is required for ordinary labels beyond existing function-
local block labels.

##### 7.1 Lowering Notes (Target Independent)

Charon maps each user label to a unique function-local TIIR block label and
rewrites references (`goto`) as branch targets to that block.

##### 7.1 Grammar / In-Memory Impact

- frontend-only labeled-statement syntax handling
- no new TIIR instruction kinds required
- existing function block table and branch-target resolution are reused

##### 7.1 Validation Notes

- label names must be unique within a function
- label references must resolve within the same function

#### 7.2 Compound Statements

##### 7.2 Feature

C99 compound statements (`{ ... }`) define block scope and statement sequencing.
charon lowers these by emitting scoped declaration/statement order into existing
TIIR blocks and instruction sequences.

No dedicated TIIR compound-statement instruction is introduced.

##### 7.2 Lowering Notes (Target Independent)

Block entry/exit semantics are preserved through declaration lifetime handling
(for example local `alloca`/`dealloca` policies) and ordered instruction
emission. Nested blocks map to dominance/scoping in existing function CFG.

##### 7.2 Grammar / In-Memory Impact

- frontend-only `{ ... }` syntax handling
- no new TIIR instruction forms
- existing scope stacks, symbol tables, and CFG blocks are reused

##### 7.2 Validation Notes

- block-scope name visibility and shadowing rules must be enforced before
  emission
- scope-exit cleanup/lifetime edges must remain well-formed

#### 7.3 Expression and Null Statements

##### 7.3 Feature

C99 expression statements (`expr;`) and null statements (`;`) are lowered by
charon into existing TIIR expression-evaluation instructions, or no emission
when a null statement has no side effects.

##### 7.3 Lowering Notes (Target Independent)

- expression statement: emit expression side effects; discard resulting value if
  unused
- null statement: emit no instruction unless required as CFG placeholder during
  structured control-flow lowering

##### 7.3 Grammar / In-Memory Impact

- frontend-only statement-surface handling
- no new TIIR instruction forms required

##### 7.3 Validation Notes

- expression side effects must be preserved even when value result is unused

#### 7.4 Selection Statements: if/else, switch

##### 7.4 Feature

Selection statements are lowered into explicit control flow using existing TIIR
branch instructions.

- `if` / `if-else` -> condition evaluation + conditional branch + merge block
- `switch` -> controlling expression evaluation + explicit `switch` dispatch +
  case/default blocks + merge/exit

TIIR adds an explicit `switch` dispatch instruction to make case lowering more
direct and analyzable.

##### 7.4 Lowering Notes (Target Independent)

For `if`/`else`, charon emits condition-to-`i1` conversion, then `br` to true/
false blocks with explicit merge. For `switch`, charon emits one `switch`
instruction over the controlling operand with an operand-pack of case target
entries plus one default target.

##### 7.4 Grammar / In-Memory Impact

- frontend-only `if`, `else`, `switch` syntax handling
- TIIR reuses existing block labels and branch instructions, and adds explicit
  `switch` dispatch form
- no additional statement-level node families are required

Proposed TIIR form:

- `switch type cond, %default, (case_value_0, %label_0, case_value_1, %label_1, ...)`

Canonical TIIR snippet:

```tiir
symbol @classify : (%x: i32) -> i32 [!linkage: external]:
%classify_bb0:
  switch i32 %x, %case_default, (0, %case_0, 1, %case_1)

%case_0:
  ret 100

%case_1:
  ret 200

%case_default:
  ret -1
```

##### 7.4 Validation Notes

- selection conditions must be convertible to boolean context
- switch controlling expression must be integer-compatible
- duplicate case values in same switch are invalid
- at most one default label per switch is allowed
- each case target label must resolve in the current function

#### 7.5 Iteration Statements: while, do-while, for

##### 7.5 Feature

Iteration statements are lowered by charon into explicit loop CFG regions using
existing labels and branches.

- `while` -> header test block, body block, exit block
- `do-while` -> body block, post-test block, exit block
- `for` -> init sequence, test block, body block, step block, exit block

No dedicated TIIR loop instruction is introduced.

##### 7.5 Lowering Notes (Target Independent)

charon emits normalized loop skeletons with explicit back-edges using `b`/`br`.
Loop-local scope/lifetime semantics are represented using existing declaration
and memory-lifetime mechanisms.

##### 7.5 Grammar / In-Memory Impact

- frontend-only loop-surface syntax handling
- TIIR reuses existing CFG blocks and branch instructions
- no new instruction families required

##### 7.5 Validation Notes

- loop condition must be boolean-convertible
- loop step/init ordering must preserve C99 execution semantics
- `continue` targets loop-specific continuation block; `break` targets loop exit

#### 7.6 Jump Statements: goto, continue, break, return

##### 7.6 Feature

Jump statements are lowered to existing TIIR branch/return instructions.

- `goto label` -> unconditional branch `b %label`
- `continue` -> branch to loop continuation point
- `break` -> branch to nearest switch/loop exit block
- `return` -> existing `ret` form (with/without operand)

No new TIIR jump instruction kinds are introduced.

##### 7.6 Lowering Notes (Target Independent)

charon resolves jump targets during CFG construction and emits explicit branch
edges. Return expression typing follows existing function return-type rules from
section 4.5 and section 6 typing constraints.

##### 7.6 Grammar / In-Memory Impact

- frontend-only jump-surface syntax handling
- TIIR reuses `b`, `br`, and `ret`
- no new core TIIR entities required

##### 7.6 Validation Notes

- `goto` target must resolve in current function
- `break` and `continue` are valid only within required statement contexts
- `return` operand presence/type must match enclosing function signature

#### 7.7 Switch case/default Semantics and Fallthrough

##### 7.7 Feature

Switch case/default and fallthrough semantics are frontend-resolved by charon
and lowered into existing labeled blocks and branches.

Fallthrough is represented by absence of terminating branch/return at the end of
a case block, allowing control to flow to the next case block in emitted order.

##### 7.7 Lowering Notes (Target Independent)

charon emits case blocks in deterministic order and one explicit `switch`
dispatch instruction, then models intentional fallthrough by preserving block
sequencing without forced branch insertion when no terminating jump exists.

##### 7.7 Grammar / In-Memory Impact

- frontend-only case/default parsing and grouping
- TIIR reuses existing labels/branches and the explicit `switch` dispatch form
- no additional switch-specific node families are required beyond instruction
  payload for case table entries

##### 7.7 Validation Notes

- case labels in one switch must be unique constant expressions
- default label appears at most once per switch
- unreachable/duplicate case diagnostics follow configured diagnostic policy
- fallthrough behavior must preserve C99 semantics unless diagnostics profile
  enforces explicit annotation policy

### 8. Functions and Calls

#### 8.1 Function Definition Structure

##### 8.1 Feature

Function definitions are frontend-structured by charon and lowered into existing
TIIR function symbols, parameter bindings, basic blocks, and instructions.

No additional function-definition instruction kind is required in TIIR beyond
existing symbol/function header and body block forms.

##### 8.1 Lowering Notes (Target Independent)

charon emits one function symbol definition header, creates parameter symbols in
function-local scope, and lowers statements/expressions into existing CFG and
instruction forms.

##### 8.1 Grammar / In-Memory Impact

- frontend-only parsing of C function-definition syntax
- TIIR reuses existing function symbol definition form from section 4.5
- no new core instruction families required

##### 8.1 Validation Notes

- function body must be type-consistent with declared signature
- all return paths must satisfy return-type constraints

#### 8.2 Old-Style vs Prototype Declarations

##### 8.2 Feature

C99 prototype vs old-style declaration differences are normalized by charon
before TIIR emission. TIIR function signatures remain explicit and canonical.

No dedicated TIIR old-style declaration form is introduced.

##### 8.2 Lowering Notes (Target Independent)

charon resolves parameter typing/defaulting semantics for non-prototype forms,
then emits a fully explicit function type/signature in TIIR.

##### 8.2 Grammar / In-Memory Impact

- frontend-only handling for old-style C declarator syntax
- TIIR reuses explicit function signature grammar from section 4.5
- no new TIIR node families required

##### 8.2 Validation Notes

- redeclaration compatibility checks apply on normalized explicit signatures
- incompatible prototype/definition forms are diagnosed pre-emission

#### 8.3 Parameter Passing and Default Promotions

##### 8.3 Feature

Parameter passing rules and default argument promotions are handled by charon
before call lowering. TIIR call sites always carry explicitly typed operands and
operand-pack argument lists.

##### 8.3 Lowering Notes (Target Independent)

charon applies required promotions/conversions to call arguments, then emits
existing TIIR call form with typed function signature and argument operand-pack.

##### 8.3 Grammar / In-Memory Impact

- frontend-only promotion policy handling
- TIIR reuses existing call syntax: call function_type dest, callee, (args)
- no new call instruction variants required

##### 8.3 Validation Notes

- argument count/type must match normalized callee signature for non-variadic
  calls
- conversion insertion must be explicit before emission where required

#### 8.4 Variadic Functions and va_list Interoperability

##### 8.4 Feature

Variadic functions are not expressible with the current fixed-arity function
type form, so TIIR adds an explicit variadic marker in function type grammar.
charon lowers C variadic declarations/calls onto this form.

Proposed TIIR variadic forms:

- function type: (param_types, ...) -> return_type
- call: call (fixed_param_types, ...) -> return_type dest, callee, (fixed_args,
  var_args...)

##### 8.4 Lowering Notes (Target Independent)

charon validates fixed-prefix arguments against declared fixed parameters, then
emits remaining operands in-order as variadic tail arguments in the same
operand-pack. va_list interoperability remains frontend/runtime-lowering policy
using existing calls/extern symbols and metadata as needed.

##### 8.4 Grammar / In-Memory Impact

- extends function type grammar with variadic marker ...
- extends call validation rules to support fixed-prefix + variadic tail
- no new instruction opcode required; existing call instruction is reused

##### 8.4 Validation Notes

- variadic call must satisfy fixed parameter count/type prefix exactly
- default argument promotions for variadic tail arguments are applied by charon
- non-variadic function symbols cannot receive extra call arguments

#### 8.5 Return Value Semantics (Including Struct/Union Return)

##### 8.5 Feature

Return semantics are lowered onto existing ret instruction forms and existing
type rules.

- void-returning functions use bare ret
- non-void functions use ret value
- aggregate return (struct/union) is frontend-lowered using existing ABI policy
  strategy (direct value return or hidden sret-style pointer parameter), without
  requiring new core TIIR statement entities

##### 8.5 Lowering Notes (Target Independent)

charon chooses aggregate-return strategy per target/runtime profile and emits
ordinary parameter/call/ret constructs accordingly. TIIR remains explicit about
the chosen lowered signature and return operation.

##### 8.5 Grammar / In-Memory Impact

- no new ret instruction kinds required beyond existing ret forms
- possible signature normalization for aggregate-return lowering is represented
  using existing function/call type nodes

##### 8.5 Validation Notes

- ret operand presence/type must match lowered function signature
- all control-flow exit paths must satisfy return constraints

#### 8.6 Recursion and Reentrancy Assumptions

##### 8.6 Feature

Recursion is supported using existing function symbol/call semantics. A
function's own symbol name is available within its body, enabling direct
recursive calls after declaration/definition point normalization by charon.

Reentrancy assumptions are target/runtime policy concerns and do not require new
TIIR core entities.

##### 8.6 Lowering Notes (Target Independent)

charon ensures the enclosing function symbol is resolvable within the function
body scope for call lowering, so self-calls lower as ordinary call instructions
to the same global symbol.

##### 8.6 Grammar / In-Memory Impact

- no new grammar productions required
- no new instruction forms required
- existing global symbol table and call resolution paths are reused

##### 8.6 Validation Notes

- recursive call signatures must match declared/lowered function signature
- unresolved self-reference is invalid
- reentrancy/thread-safety diagnostics remain optional profile checks

### 9. Objects, Memory, and Layout Semantics

#### 9.1 Object Representation and Alignment

##### 9.1 Feature

TIIR remains target-independent at type surface level, while concrete object
size, alignment, and field offsets are determined in Pluto using the selected
target ABI/data model.

Target ABI is authoritative for:

- scalar size/alignment (for example `i32`, `f64`, pointer width)
- struct/union layout rules
- stack/object alignment requirements
- parameter/return object passing layout decisions

##### 9.1 Lowering Notes (Target Independent)

charon emits target-independent TIIR types/symbols; Pluto performs ABI layout
computation when lowering TIIR to concrete object/memory representations,
attaching ABI-derived layout metadata to relevant objects/types as needed.
Existing TIIR entities are reused; layout is a Pluto-lowering interpretation
layer, not a new core type syntax.

##### 9.1 Grammar / In-Memory Impact

- no new core TIIR type syntax required for ABI layout
- optional layout metadata payloads may be attached in implementation
- existing type and symbol nodes are reused

##### 9.1 Validation Notes

- ABI-derived size/alignment must be internally consistent for all emitted
  objects
- misaligned object accesses that violate ABI constraints must be diagnosed or
  lowered via target-defined safe access strategy

#### 9.2 Padding and Trap Representations

##### 9.2 Feature

Padding bytes and trap representations are target/ABI-defined concerns. TIIR
does not require explicit padding syntax; Pluto layout/initialization lowering
must account for ABI-required padding and representation hazards.

Additionally, TIIR defines an explicit trap operation for paths that must become
immediate runtime termination or unreachable behavior.

Proposed forms:

- opcode form: `trap`
- builtin alternative: `call () -> void %0, @__pluto_trap, ()` (frontend/runtime
  policy)

##### 9.2 Lowering Notes (Target Independent)

Pluto computes and preserves implicit padding per ABI layout rules when mapping
aggregate initializers and memory copies. Where trap-representation risk exists,
Pluto may emit explicit `trap` or configured diagnostics per safety/profile
policy.

##### 9.2 Grammar / In-Memory Impact

- adds explicit `trap` instruction form (or equivalent builtin lowering path)
- optional metadata/diagnostic annotations may describe padding-sensitive paths

##### 9.2 Validation Notes

- aggregate initializer lowering must not assign semantic values to padding
  bytes unless explicitly required by target policy
- potential trap-representation reads/writes follow configured diagnostic policy
- emitted `trap` must terminate current control-flow path (no fallthrough)

#### 9.3 Strict Aliasing Constraints

##### 9.3 Feature

Strict aliasing is a language/optimization constraint resolved by charon
analysis and downstream optimization policy. TIIR memory operations remain
typed (`load/store/gep`) and provide the base information for alias reasoning
without introducing new core aliasing syntax.

##### 9.3 Lowering Notes (Target Independent)

charon may attach alias-relevant metadata where needed, but canonical lowering
continues to use existing pointer/typed memory instructions.

##### 9.3 Grammar / In-Memory Impact

- no new TIIR core grammar required
- existing typed memory operations and type metadata are reused

##### 9.3 Validation Notes

- aliasing-unsafe transformations are disallowed unless justified by language
  and profile constraints
- diagnostics for suspicious aliasing patterns are profile-controlled

#### 9.4 Effective Type Rules

##### 9.4 Feature

Effective type semantics are handled by charon semantic analysis and memory-use
validation, then reflected through existing typed memory operations in TIIR.

##### 9.4 Lowering Notes (Target Independent)

charon tracks object effective type updates (for example through stores and
initialization) and ensures emitted `load/store` operations are type-consistent
with language rules or explicitly diagnosed where policy requires.

##### 9.4 Grammar / In-Memory Impact

- no new TIIR instruction syntax required
- optional analysis metadata may be attached for effective-type tracking

##### 9.4 Validation Notes

- type-incoherent memory access relative to effective type must be diagnosed or
  lowered under explicit compatibility policy
- aggregate/member accesses must remain consistent with computed layout and
  declared type interpretation

#### 9.5 Sequence Points and Side-Effect Ordering

##### 9.5 Feature

Sequence-point/side-effect ordering constraints are preserved by charon during
expression lowering to ordered TIIR instruction streams and CFG edges.

##### 9.5 Lowering Notes (Target Independent)

charon linearizes expression evaluation according to C99 sequencing rules,
ensuring stores/calls/volatile accesses are emitted in semantically correct
order. Existing TIIR instruction ordering plus explicit branches encode the
required execution order.

##### 9.5 Grammar / In-Memory Impact

- no new sequencing instruction forms required
- existing instruction order and CFG structure are reused

##### 9.5 Validation Notes

- unsequenced/undefined side-effect conflicts should trigger diagnostics per
  configured language conformance level
- optimization phases must preserve required observable order constraints

#### 9.6 Volatile Access Semantics

##### 9.6 Feature

Volatile semantics are preserved through charon lowering and existing typed
memory operations, with volatility represented via existing qualifier/metadata
model.

##### 9.6 Lowering Notes (Target Independent)

charon prevents elimination/reordering of volatile accesses beyond what language
and target memory model permit. Emitted volatile loads/stores use existing
memory operations annotated by existing qualifier metadata.

##### 9.6 Grammar / In-Memory Impact

- no new core TIIR volatile opcode required
- existing `!qualifiers: volatile` and typed load/store forms are reused

##### 9.6 Validation Notes

- volatile object accesses must remain observable in program order per language
  semantics and target memory model constraints
- transformations that drop or merge volatile accesses are invalid

### 10. Diagnostics and Constraints

#### 10.1 Required Diagnostics for Constraint Violations

##### 10.1 Feature

Constraint violations must produce diagnostics according to language conformance
and profile settings. charon is responsible for source-level diagnostics during
parsing/semantic analysis; Pluto may emit additional lowering-time diagnostics
for ABI/layout/codegen constraints.

Diagnostic severities:

- `error`: translation/lowering cannot proceed for the affected unit/path
- `warning`: translation may proceed, behavior/policy risk is reported
- `note`: supplementary context attached to warning/error diagnostics

##### 10.1 Lowering Notes (Target Independent)

charon should emit precise source-located diagnostics for syntax, type,
declaration, and semantic constraint violations before TIIR emission whenever
possible. Pluto diagnostics should reference lowered entities and, where
available, map back to source locations/metadata.

##### 10.1 Grammar / In-Memory Impact

- no new core TIIR grammar required
- optional diagnostic metadata payloads may be attached by tools/passes
- existing source-location metadata hooks are reused

##### 10.1 Validation Notes

- diagnostics required by this spec must not be silently suppressed
- configured diagnostic profiles may escalate/demote selected classes where
  allowed, but cannot downgrade mandatory hard-failure constraints below error
- first-failure and aggregate-report modes are both permitted by implementation
  policy

#### 10.2 Undefined, Unspecified, and Implementation-Defined Behavior

##### 10.2 Feature

Behavior classes follow C99 taxonomy and must be tracked explicitly by frontend
and lowering policy:

- undefined behavior (UB): no language-mandated result
- unspecified behavior: one of several permitted results, not fixed per run
- implementation-defined behavior: implementation chooses behavior and should
  document the choice

##### 10.2 Lowering Notes (Target Independent)

charon should identify UB/unspecified/implementation-defined sites when
determinable at compile time. Pluto must apply target/profile policy for
lowering outcomes (for example emit diagnostics, preserve behavior as-is,
insert guards, or emit `trap` where configured).

##### 10.2 Grammar / In-Memory Impact

- no new core grammar required
- optional behavior-class annotations/metadata may be attached for analysis and
  reporting

##### 10.2 Validation Notes

- UB sites detected statically should produce diagnostics per profile (at least
  warning by default, error when strict mode requires)
- implementation-defined choices used by Pluto/charon should be discoverable in
  toolchain documentation/output mode
- unspecified behavior should not be falsely stabilized unless an explicit
  profile policy mandates deterministic lowering

#### 10.3 Translation Limits Affecting Frontend and IR Generation

##### 10.3 Feature

Translation limits bound parser, semantic, and IR generation resources (for
example nesting depth, symbol counts, initializer complexity, and expression
size). Limits may be profile-configurable but must produce deterministic
diagnostics when exceeded.

##### 10.3 Lowering Notes (Target Independent)

charon enforces source-facing limits during parsing/semantic phases; Pluto may
enforce additional backend/lowering limits tied to target ABI/object format.
Both should emit clear diagnostics indicating the exceeded limit and active
profile/configuration.

##### 10.3 Grammar / In-Memory Impact

- no new core TIIR grammar required
- optional per-module/profile metadata may record active limit settings

##### 10.3 Validation Notes

- limit exceedance must produce diagnostics with category, threshold, and
  observed value where available
- implementations may support recovery/continuation after non-fatal limit
  diagnostics, but hard resource/consistency failures must terminate affected
  translation path
- limit defaults and override mechanisms should be documented for reproducible
  builds

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
