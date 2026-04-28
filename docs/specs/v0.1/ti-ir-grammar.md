# TI-IR Text Grammar (v0.1 Draft)

Status: Draft
Scope: Text serialization grammar for TI-IR v0.1.

## Goals

1. Deterministic and human-readable text format.
2. Symbolic cross-reference syntax with compiler-assigned internal stable IDs.
3. Round-trip stable parse and emit.
4. Clear verifier anchors for structure, SSA, and type checks.

## Entity Skeleton

1. Module
2. Type declarations
3. Constant declarations
4. Function declarations
5. Basic blocks
6. Instructions
7. Values (named SSA bindings and unnamed temporaries)

## Full EBNF Grammar (Draft)

```ebnf
file            = { meta_decl }, module ;
meta_decl       = ident, string_literal, ";" ;

module         = "module", ident, "{" { module_decl } "}" ;
module_decl    = type_decl | const_decl | function_decl ;

type_decl      = "type", ident, "=", type_expr, ";" ;
const_decl     = "const", ident, ":", type_expr, "=", literal, ";" ;

function_decl  = "fn", ident, function_sig, "{", { block_decl }, "}" ;
function_sig   = "(", [ param_list ], ")", "->", type_expr ;
param_list     = param, { ",", param } ;
param          = ident, ":", type_expr ;

block_decl     = ident, ":", { inst_decl } ;
inst_decl      = [ result_pattern, ":", type_expr, "=" ], opcode, operand_list, ";" ;
operand_list   = [ operand, { ",", operand } ] ;
operand        = name | literal ;

result_pattern = name | tuple_literal ;

name           = ident | "@", integral_literal ;
ident          = letter, { letter | digit | "_" } ;

type_expr      = tuple_type | struct_type | name | primitive_type ;
tuple_type     = "(", type_expr, ",", type_expr, { ",", type_expr }, ")" ;
struct_type    = "struct", "{", field_decl, { ",", field_decl }, "}" ;
field_decl     = ident, ":", type_expr ;

primitive_type = "i8" | "i16" | "i32" | "i64"
               | "u8" | "u16" | "u32" | "u64"
               | "f32" | "f64"
               | "bool" | "nil" ;

literal            = tuple_literal | scalar_literal ;
tuple_literal      = "(", [ operand, { ",", operand }, ] ")" ;
scalar_literal     = integral_literal | string_literal | "true" | "false" ;
integral_literal   = digit, { digit } ;
```

## Grammar Forms (Draft)

### 1. File Metadata Form

```ebnf
file            = { meta_decl }, module ;
meta_decl       = ident, string_literal, ";" ;
```

Semantics:
- File metadata declarations are key-value entries at file scope.
- Detailed per-key semantics are specified in [Metadata Declarations](meta-decls.md).

### 2. Module Form

```ebnf
module         = "module", ident, "{" { module_decl } "}" ;
module_decl    = type_decl | const_decl | function_decl ;
```

Semantics:
- Module form semantics are specified in [Module Semantics](modules.md).

### 3. Type Declaration Form

```ebnf
type_decl      = "type", ident, "=", type_expr, ";" ;
```

Semantics:
- Type declaration semantics are specified in [Type Semantics](types.md).

### 4. Constant Declaration Form

```ebnf
const_decl     = "const", ident, ":", type_expr, "=", literal, ";" ;
```

Semantics:
- Constant semantics are specified in [Constant Semantics](constants.md).

### 5. Function Form

```ebnf
function_decl  = "fn", ident, function_sig, "{", { block_decl }, "}" ;
function_sig   = "(", [ param_list ], ")", "->", type_expr ;
param_list     = param, { ",", param } ;
param          = ident, ":", type_expr ;
```

Semantics:
- Function semantics are specified in [Function Semantics](functions.md).

### 6. Block Form

```ebnf
block_decl     = ident, ":", { inst_decl } ;
```

Semantics:
- Basic block semantics are specified in [Basic Block Semantics](basic-blocks.md).

### 7. Instruction Form

```ebnf
inst_decl      = [ result_pattern, ":", type_expr, "=" ], opcode, operand_list, ";" ;
operand_list   = [ operand, { ",", operand } ] ;
operand        = name | literal ;
```

Semantics:
- Instruction semantics are specified in [Instruction Semantics](instructions.md).

### 8. Name Form

```ebnf
name           = ident | "@", integral_literal ;
ident          = letter, { letter | digit | "_" } ;
```

Semantics:
- Name semantics are specified in [Name Semantics](names.md).


### 9. Type Expression Form

```ebnf
type_expr      = tuple_type | struct_type | name | primitive_type ;
tuple_type     = "(", type_expr, ",", type_expr, { ",", type_expr }, ")" ;
struct_type    = "struct", "{", field_decl, { ",", field_decl }, "}" ;
field_decl     = ident, ":", type_expr ;
primitive_type = "i8" | "i16" | "i32" | "i64"
               | "u8" | "u16" | "u32" | "u64"
               | "f32" | "f64"
               | "bool" | "nil" ;
```

Semantics:
- Type expression semantics are specified in [Type Semantics](types.md).

### 10. Literal Form

```ebnf
literal        = tuple_literal | scalar_literal ;
tuple_literal  = "(", [ operand, { ",", operand }, ] ")" ;
scalar_literal = integral_literal | string_literal | "true" | "false" ;
integral_literal         = digit, { digit } ;
```

Semantics:
- Literal semantics are specified in [Literal Semantics](literals.md).

## Example: Straight-Line Function

```
format "ti-ir";
version 0.1;

module example;

const one: i32 = 1;
const two: i32 = 2;

fn add_and_double(x: i32, y: i32) -> i32 {
entry:
   sum: i32 = add x, y;    (* named params referenced by name *)
   biased: i32 = add sum, one;  (* named local referenced by name *)
   @0: i32 = mul biased, two;   (* unnamed temporary referenced by @-name *)
   ret @0;
}
```

Notes on this example:
- Global declarations use symbolic names (`one`, `two`, `add_and_double`) rather than exposed stable IDs.
- Named parameters (`x`, `y`) and named instruction results (`sum`, `biased`) are referenced directly by name.
- `entry:` introduces a block, and the block body runs until the next block header or the end of the enclosing function.
- `@0` is an unnamed temporary: it has SSA identity within the function but no user-facing symbolic name.
- `ret` is a terminator with no result binding and no `:` annotation — `@0`'s type is already known from its definition.

Opcode set note for v0.1:
- Valid opcodes are `add`, `sub`, `mul`, `div`, `mod`, and `ret`.
- More complex control-flow and interprocedural opcodes (for example `br`, `phi`, and `call`) are out of scope.

## Example: Type Alias Composition

```
format "ti-ir";
version 0.1;

module type_alias_demo;

type i32_word = i32;
const bias: i32_word = 1;

fn add_with_bias(left: i32_word, right: i32_word) -> i32_word {
entry:
   sum: i32_word = add left, right;
   biased: i32_word = add sum, bias;
   ret biased;
}
```

Notes on this example:
- `i32_word` aliases a primitive type (`i32`) and is reused in declarations and instruction result typing.
- The function signature and instruction result type use symbolic type names directly in type positions.
- Value names (`left`, `right`, `sum`, `biased`) are direct SSA bindings in the source text.

## Open Questions

1. Should instruction aliases ever exist, or should readability stop at type/function/block naming?
2. Should tuple result patterns be fully activated only after additional opcodes are introduced?

## Next Edits

1. Lock lexical tokens, alias rules, and escaping rules.
2. Define precise operand typing constraints for `add`, `sub`, `mul`, `div`, `mod`, and `ret`.
3. Add at least three canonical examples using explicit version metadata and aliases:
   1. Straight-line block.
   2. Arithmetic chain using only the v0.1 opcode set.
   3. Invalid program expected to fail verifier (for example out-of-scope opcode use).
