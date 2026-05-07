// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef PLUTO_TIIR_VALUE_H
#define PLUTO_TIIR_VALUE_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

struct _pluto_Value;
struct _pluto_ValueTuple;
struct _pluto_ValueArray;
typedef struct _pluto_Value *pluto_Value;
typedef struct _pluto_ValueTuple *pluto_ValueTuple;
typedef struct _pluto_ValueArray *pluto_ValueArray;

typedef enum _pluto_ValueKind {
    PLUTO_VALUE_KIND_NIL,
    PLUTO_VALUE_KIND_BOOL,
    PLUTO_VALUE_KIND_I8,
    PLUTO_VALUE_KIND_I16,
    PLUTO_VALUE_KIND_I32,
    PLUTO_VALUE_KIND_I64,
    PLUTO_VALUE_KIND_U8,
    PLUTO_VALUE_KIND_U16,
    PLUTO_VALUE_KIND_U32,
    PLUTO_VALUE_KIND_U64,
    PLUTO_VALUE_KIND_F32,
    PLUTO_VALUE_KIND_F64,
    PLUTO_VALUE_KIND_STRING,
    PLUTO_VALUE_KIND_TUPLE,
    PLUTO_VALUE_KIND_ARRAY,
} pluto_ValueKind;

pluto_Value pluto_value_construct_nil();
pluto_Value pluto_value_construct_bool(bool b);
pluto_Value pluto_value_construct_i8(int8_t i);
pluto_Value pluto_value_construct_i16(int16_t i);
pluto_Value pluto_value_construct_i32(int32_t i);
pluto_Value pluto_value_construct_i64(int64_t i);
pluto_Value pluto_value_construct_u8(uint8_t u);
pluto_Value pluto_value_construct_u16(uint16_t u);
pluto_Value pluto_value_construct_u32(uint32_t u);
pluto_Value pluto_value_construct_u64(uint64_t u);
pluto_Value pluto_value_construct_f32(float f);
pluto_Value pluto_value_construct_f64(double f);
pluto_Value pluto_value_construct_string(uint32_t string_tag);
pluto_Value pluto_value_construct_tuple();
pluto_Value pluto_value_construct_array();

void pluto_value_destruct(pluto_Value *value);

pluto_ValueKind pluto_value_kind(pluto_Value value);
bool pluto_value_is(pluto_Value value, pluto_ValueKind kind);
bool pluto_value_get_bool(pluto_Value value);
int8_t pluto_value_get_i8(pluto_Value value);
int16_t pluto_value_get_i16(pluto_Value value);
int32_t pluto_value_get_i32(pluto_Value value);
int64_t pluto_value_get_i64(pluto_Value value);
uint8_t pluto_value_get_u8(pluto_Value value);
uint16_t pluto_value_get_u16(pluto_Value value);
uint32_t pluto_value_get_u32(pluto_Value value);
uint64_t pluto_value_get_u64(pluto_Value value);
float pluto_value_get_f32(pluto_Value value);
double pluto_value_get_f64(pluto_Value value);
uint32_t pluto_value_get_string_tag(pluto_Value value);
pluto_ValueTuple pluto_value_get_tuple(pluto_Value value);
pluto_ValueArray pluto_value_get_array(pluto_Value value);

void pluto_value_tuple_append(pluto_ValueTuple tuple, pluto_Value value);
pluto_Value pluto_value_tuple_at(pluto_ValueTuple tuple, size_t index);
size_t pluto_value_tuple_length(pluto_ValueTuple tuple);
void pluto_value_array_append(pluto_ValueArray array, pluto_Value value);
pluto_Value pluto_value_array_at(pluto_ValueArray array, size_t index);
size_t pluto_value_array_length(pluto_ValueArray array);

struct _pluto_Context;
void pluto_value_print(FILE *out, pluto_Value value, struct _pluto_Context *ctx);

#endif // !PLUTO_TIIR_VALUE_H
