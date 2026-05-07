// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef PLUTO_TIIR_LITERAL_H
#define PLUTO_TIIR_LITERAL_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

struct _pluto_Literal;
struct _pluto_LiteralTuple;
struct _pluto_LiteralArray;
typedef struct _pluto_Literal *pluto_Literal;
typedef struct _pluto_LiteralTuple *pluto_LiteralTuple;
typedef struct _pluto_LiteralArray *pluto_LiteralArray;

typedef enum _pluto_LiteralKind
{
    PLUTO_LITERAL_KIND_NIL,
    PLUTO_LITERAL_KIND_BOOL,
    PLUTO_LITERAL_KIND_I8,
    PLUTO_LITERAL_KIND_I16,
    PLUTO_LITERAL_KIND_I32,
    PLUTO_LITERAL_KIND_I64,
    PLUTO_LITERAL_KIND_U8,
    PLUTO_LITERAL_KIND_U16,
    PLUTO_LITERAL_KIND_U32,
    PLUTO_LITERAL_KIND_U64,
    PLUTO_LITERAL_KIND_F32,
    PLUTO_LITERAL_KIND_F64,
    PLUTO_LITERAL_KIND_STRING,
    PLUTO_LITERAL_KIND_TUPLE,
    PLUTO_LITERAL_KIND_ARRAY,
} pluto_LiteralKind;

pluto_Literal pluto_literal_construct_nil();
pluto_Literal pluto_literal_construct_bool(bool b);
pluto_Literal pluto_literal_construct_i8(int8_t i);
pluto_Literal pluto_literal_construct_i16(int16_t i);
pluto_Literal pluto_literal_construct_i32(int32_t i);
pluto_Literal pluto_literal_construct_i64(int64_t i);
pluto_Literal pluto_literal_construct_u8(uint8_t u);
pluto_Literal pluto_literal_construct_u16(uint16_t u);
pluto_Literal pluto_literal_construct_u32(uint32_t u);
pluto_Literal pluto_literal_construct_u64(uint64_t u);
pluto_Literal pluto_literal_construct_f32(float f);
pluto_Literal pluto_literal_construct_f64(double f);
pluto_Literal pluto_literal_construct_string(uint32_t string_tag);
pluto_Literal pluto_literal_construct_tuple();
pluto_Literal pluto_literal_construct_array();

void pluto_literal_destruct(pluto_Literal *value);

pluto_LiteralKind pluto_literal_kind(pluto_Literal value);
bool pluto_literal_is(pluto_Literal value, pluto_LiteralKind kind);
bool pluto_literal_get_bool(pluto_Literal value);
int8_t pluto_literal_get_i8(pluto_Literal value);
int16_t pluto_literal_get_i16(pluto_Literal value);
int32_t pluto_literal_get_i32(pluto_Literal value);
int64_t pluto_literal_get_i64(pluto_Literal value);
uint8_t pluto_literal_get_u8(pluto_Literal value);
uint16_t pluto_literal_get_u16(pluto_Literal value);
uint32_t pluto_literal_get_u32(pluto_Literal value);
uint64_t pluto_literal_get_u64(pluto_Literal value);
float pluto_literal_get_f32(pluto_Literal value);
double pluto_literal_get_f64(pluto_Literal value);
uint32_t pluto_literal_get_string_tag(pluto_Literal value);
pluto_LiteralTuple pluto_literal_get_tuple(pluto_Literal value);
pluto_LiteralArray pluto_literal_get_array(pluto_Literal value);

void pluto_literal_tuple_append(pluto_LiteralTuple tuple, pluto_Literal value);
pluto_Literal pluto_literal_tuple_at(pluto_LiteralTuple tuple, size_t index);
size_t pluto_literal_tuple_length(pluto_LiteralTuple tuple);
void pluto_literal_array_append(pluto_LiteralArray array, pluto_Literal value);
pluto_Literal pluto_literal_array_at(pluto_LiteralArray array, size_t index);
size_t pluto_literal_array_length(pluto_LiteralArray array);

bool pluto_literal_eq(pluto_Literal a, pluto_Literal b);

struct _pluto_Context;
void pluto_literal_print(FILE *out, pluto_Literal value, struct _pluto_Context *ctx);

#endif // !PLUTO_TIIR_LITERAL_H
