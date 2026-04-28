# TI-IR Type Semantics (v0.1 Draft)

Status: Draft
Scope: Semantics for type declarations and type expressions in TI-IR text files.

## Grammar

```ebnf
type_decl      = "type", ident, "=", type_expr, ";" ;

type_expr      = tuple_type | struct_type | name | primitive_type ;
tuple_type     = "(", type_expr, ",", type_expr, { ",", type_expr }, ")" ;
struct_type    = "struct", "{", field_decl, { ",", field_decl }, "}" ;
field_decl     = ident, ":", type_expr ;
primitive_type = "i8" | "i16" | "i32" | "i64"
               | "u8" | "u16" | "u32" | "u64"
               | "f32" | "f64"
               | "bool" | "nil" ;
```

## Semantics

### Type Declarations

- `type` introduces a symbolic alias bound to a `type_expr`.

### Type Expressions

### Primitive Types

- Primitive types are first-class and may appear in any type position.

### Tuple Types

- Tuple types are positional product types.
- Arity is determined by the number of elements.
- `()` denotes the empty tuple and has type `nil`.
- Parenthesized single values are not tuple types of arity 1.

### Struct Types

- `struct` types are nominally anonymous in the expression form and structurally defined by ordered fields.
- Field names and field types are part of the struct type shape.
- `struct {}` is semantically equivalent to `nil`.

### Named Type References

- `name` in type position resolves to a previously visible type alias or a qualified type symbol.
- Resolution failures are semantic validation errors.

## Contract

- Aliases may name any valid `type_expr`.
- Alias resolution follows normal visibility and ordering rules of module declarations.
- Redefinition of an existing type name in the same module scope is invalid unless future rules introduce explicit shadowing or versioning behavior.

## Cross-Cutting Concerns

- In `const_decl`, the declared type must match or admit the initializer literal by type-checking rules.
- In `function_sig`, parameter and return `type_expr` forms are validated using these same type semantics.
- In `inst_decl`, the annotated `type_expr` constrains opcode and operand typing.
