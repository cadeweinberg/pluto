// SPDX-License-Identifier: GPL-3.0-or-later

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "tiir/value.h"
#include "context/context.h"

struct _pluto_ValueTuple {
    pluto_Value *values;
    size_t length;
    size_t capacity;
};

struct _pluto_ValueArray {
    pluto_Value *values;
    size_t length;
    size_t capacity;
};

struct _pluto_Value {
    pluto_ValueKind kind;
    union {
        bool b;
        int8_t i8;
        int16_t i16;
        int32_t i32;
        int64_t i64;
        uint8_t u8;
        uint16_t u16;
        uint32_t u32;
        uint64_t u64;
        float f32;
        double f64;
        uint32_t string_tag;
        struct _pluto_ValueTuple tuple;
        struct _pluto_ValueArray array;
    } data;
};

pluto_Value _pluto_value_construct() {
    return calloc(1, sizeof(struct _pluto_Value));
}

void pluto_value_destruct(pluto_Value *value) {
    if (value && *value) {
        switch ((*value)->kind) {
            case PLUTO_VALUE_KIND_TUPLE:
                for (size_t i = 0; i < (*value)->data.tuple.length; ++i) {
                    pluto_value_destruct(&(*value)->data.tuple.values[i]);
                }
                free((*value)->data.tuple.values);
                break;
            case PLUTO_VALUE_KIND_ARRAY:
                for (size_t i = 0; i < (*value)->data.array.length; ++i) {
                    pluto_value_destruct(&(*value)->data.array.values[i]);
                }
                free((*value)->data.array.values);
                break;
            default:
                break;
        }
        free(*value);
        *value = NULL;
    }
}

pluto_Value pluto_value_construct_nil() {
    pluto_Value value = _pluto_value_construct();
    value->kind = PLUTO_VALUE_KIND_NIL;
    return value;
}

pluto_Value pluto_value_construct_bool(bool b) {
    pluto_Value value = _pluto_value_construct();
    value->kind = PLUTO_VALUE_KIND_BOOL;
    value->data.b = b;
    return value;
}

pluto_Value pluto_value_construct_i8(int8_t i) {
    pluto_Value value = _pluto_value_construct();
    value->kind = PLUTO_VALUE_KIND_I8;
    value->data.i8 = i;
    return value;
}

pluto_Value pluto_value_construct_i16(int16_t i) {
    pluto_Value value = _pluto_value_construct();
    value->kind = PLUTO_VALUE_KIND_I16;
    value->data.i16 = i;
    return value;
}

pluto_Value pluto_value_construct_i32(int32_t i) {
    pluto_Value value = _pluto_value_construct();
    value->kind = PLUTO_VALUE_KIND_I32;
    value->data.i32 = i;
    return value;
}

pluto_Value pluto_value_construct_i64(int64_t i) {
    pluto_Value value = _pluto_value_construct();
    value->kind = PLUTO_VALUE_KIND_I64;
    value->data.i64 = i;
    return value;
}

pluto_Value pluto_value_construct_u8(uint8_t u) {
    pluto_Value value = _pluto_value_construct();
    value->kind = PLUTO_VALUE_KIND_U8;
    value->data.u8 = u;
    return value;
}

pluto_Value pluto_value_construct_u16(uint16_t u) {
    pluto_Value value = _pluto_value_construct();
    value->kind = PLUTO_VALUE_KIND_U16;
    value->data.u16 = u;
    return value;
}

pluto_Value pluto_value_construct_u32(uint32_t u) {
    pluto_Value value = _pluto_value_construct();
    value->kind = PLUTO_VALUE_KIND_U32;
    value->data.u32 = u;
    return value;
}

pluto_Value pluto_value_construct_u64(uint64_t u) {
    pluto_Value value = _pluto_value_construct();
    value->kind = PLUTO_VALUE_KIND_U64;
    value->data.u64 = u;
    return value;
}

pluto_Value pluto_value_construct_f32(float f) {
    pluto_Value value = _pluto_value_construct();
    value->kind = PLUTO_VALUE_KIND_F32;
    value->data.f32 = f;
    return value;
}

pluto_Value pluto_value_construct_f64(double f) {
    pluto_Value value = _pluto_value_construct();
    value->kind = PLUTO_VALUE_KIND_F64;
    value->data.f64 = f;
    return value;
}

pluto_Value pluto_value_construct_string(uint32_t string_tag) {
    pluto_Value value = _pluto_value_construct();
    value->kind = PLUTO_VALUE_KIND_STRING;
    value->data.string_tag = string_tag;
    return value;
}

pluto_Value pluto_value_construct_tuple() {
    pluto_Value value = _pluto_value_construct();
    value->kind = PLUTO_VALUE_KIND_TUPLE;
    value->data.tuple.values = NULL;
    value->data.tuple.length = 0;
    value->data.tuple.capacity = 0;
    return value;
}

pluto_Value pluto_value_construct_array() {
    pluto_Value value = _pluto_value_construct();
    value->kind = PLUTO_VALUE_KIND_ARRAY;
    value->data.array.values = NULL;
    value->data.array.length = 0;
    value->data.array.capacity = 0;
    return value;
}

pluto_ValueKind pluto_value_kind(pluto_Value value) {
    assert(value != NULL);
    return value->kind;
}

bool pluto_value_is(pluto_Value value, pluto_ValueKind kind) {
    assert(value != NULL);
    return value->kind == kind;
}

bool pluto_value_get_bool(pluto_Value value) {
    assert(pluto_value_is(value, PLUTO_VALUE_KIND_BOOL));
    return value->data.b;
}

int8_t pluto_value_get_i8(pluto_Value value) {
    assert(pluto_value_is(value, PLUTO_VALUE_KIND_I8));
    return value->data.i8;
}

int16_t pluto_value_get_i16(pluto_Value value) {
    assert(pluto_value_is(value, PLUTO_VALUE_KIND_I16));
    return value->data.i16;
}

int32_t pluto_value_get_i32(pluto_Value value) {
    assert(pluto_value_is(value, PLUTO_VALUE_KIND_I32));
    return value->data.i32;
}

int64_t pluto_value_get_i64(pluto_Value value) {
    assert(pluto_value_is(value, PLUTO_VALUE_KIND_I64));
    return value->data.i64;
}

uint8_t pluto_value_get_u8(pluto_Value value) {
    assert(pluto_value_is(value, PLUTO_VALUE_KIND_U8));
    return value->data.u8;
}

uint16_t pluto_value_get_u16(pluto_Value value) {
    assert(pluto_value_is(value, PLUTO_VALUE_KIND_U16));
    return value->data.u16;
}

uint32_t pluto_value_get_u32(pluto_Value value) {
    assert(pluto_value_is(value, PLUTO_VALUE_KIND_U32));
    return value->data.u32;
}

uint64_t pluto_value_get_u64(pluto_Value value) {
    assert(pluto_value_is(value, PLUTO_VALUE_KIND_U64));
    return value->data.u64;
}

float pluto_value_get_f32(pluto_Value value) {
    assert(pluto_value_is(value, PLUTO_VALUE_KIND_F32));
    return value->data.f32;
}

double pluto_value_get_f64(pluto_Value value) {
    assert(pluto_value_is(value, PLUTO_VALUE_KIND_F64));
    return value->data.f64;
}

uint32_t pluto_value_get_string_tag(pluto_Value value) {
    assert(pluto_value_is(value, PLUTO_VALUE_KIND_STRING));
    return value->data.string_tag;
}

pluto_ValueTuple pluto_value_get_tuple(pluto_Value value) {
    assert(pluto_value_is(value, PLUTO_VALUE_KIND_TUPLE));
    return &value->data.tuple;
}

pluto_ValueArray pluto_value_get_array(pluto_Value value) {
    assert(pluto_value_is(value, PLUTO_VALUE_KIND_ARRAY));
    return &value->data.array;
}

static bool _pluto_value_tuple_full(pluto_ValueTuple tuple) {
    return tuple->length >= tuple->capacity;
}

static void _pluto_value_tuple_grow(pluto_ValueTuple tuple) {
    size_t new_capacity = tuple->capacity == 0 ? 4 : tuple->capacity * 2;
    pluto_Value *new_values = realloc(tuple->values, new_capacity * sizeof(pluto_Value));
    if (new_values) {
        tuple->values = new_values;
        tuple->capacity = new_capacity;
    }
}

void pluto_value_tuple_append(pluto_ValueTuple tuple, pluto_Value value) {
    if (_pluto_value_tuple_full(tuple)) {
        _pluto_value_tuple_grow(tuple);
    }
    tuple->values[tuple->length++] = value;
}

pluto_Value pluto_value_tuple_at(pluto_ValueTuple tuple, size_t index) {
    assert(index < tuple->length);
    return tuple->values[index];
}

size_t pluto_value_tuple_length(pluto_ValueTuple tuple) {
    return tuple->length;
}

static bool _pluto_value_array_full(pluto_ValueArray array) {
    return array->length >= array->capacity;
}

static void _pluto_value_array_grow(pluto_ValueArray array) {
    size_t new_capacity = array->capacity == 0 ? 4 : array->capacity * 2;
    pluto_Value *new_values = realloc(array->values, new_capacity * sizeof(pluto_Value));
    if (new_values) {
        array->values = new_values;
        array->capacity = new_capacity;
    }
}

void pluto_value_array_append(pluto_ValueArray array, pluto_Value value) {
    if (_pluto_value_array_full(array)) {
        _pluto_value_array_grow(array);
    }
    array->values[array->length++] = value;
}

pluto_Value pluto_value_array_at(pluto_ValueArray array, size_t index) {
    assert(index < array->length);
    return array->values[index];
}

size_t pluto_value_array_length(pluto_ValueArray array) {
    return array->length;
}

bool pluto_value_eq(pluto_Value a, pluto_Value b) {
    if (a->kind != b->kind) { return false; }
    switch (a->kind) {
        case PLUTO_VALUE_KIND_NIL:
            return true;
        case PLUTO_VALUE_KIND_BOOL:
            return a->data.b == b->data.b;
        case PLUTO_VALUE_KIND_I8:
            return a->data.i8 == b->data.i8;
        case PLUTO_VALUE_KIND_I16:
            return a->data.i16 == b->data.i16;
        case PLUTO_VALUE_KIND_I32:
            return a->data.i32 == b->data.i32;
        case PLUTO_VALUE_KIND_I64:
            return a->data.i64 == b->data.i64;
        case PLUTO_VALUE_KIND_U8:
            return a->data.u8 == b->data.u8;
        case PLUTO_VALUE_KIND_U16:
            return a->data.u16 == b->data.u16;
        case PLUTO_VALUE_KIND_U32:
            return a->data.u32 == b->data.u32;
        case PLUTO_VALUE_KIND_U64:
            return a->data.u64 == b->data.u64;
        case PLUTO_VALUE_KIND_F32:
            return a->data.f32 == b->data.f32;
        case PLUTO_VALUE_KIND_F64:
            return a->data.f64 == b->data.f64;
        case PLUTO_VALUE_KIND_STRING:
            return a->data.string_tag == b->data.string_tag;
        case PLUTO_VALUE_KIND_TUPLE: {
            if (a->data.tuple.length != b->data.tuple.length) { return false; }
            for (size_t i = 0; i < a->data.tuple.length; ++i) {
                if (!pluto_value_eq(a->data.tuple.values[i], b->data.tuple.values[i])) {
                    return false;
                }
            }
            return true;
        }
        case PLUTO_VALUE_KIND_ARRAY: {
            if (a->data.array.length != b->data.array.length) { return false; }
            for (size_t i = 0; i < a->data.array.length; ++i) {
                if (!pluto_value_eq(a->data.array.values[i], b->data.array.values[i])) {
                    return false;
                }
            }
            return true;
        }
    }
    return false;
}

void pluto_value_print(FILE *out, pluto_Value value, struct _pluto_Context *ctx) {
    assert(value != NULL);
    switch (value->kind) {
        case PLUTO_VALUE_KIND_NIL:
            fprintf(out, "nil");
            break;
        case PLUTO_VALUE_KIND_BOOL:
            fprintf(out, value->data.b ? "true" : "false");
            break;
        case PLUTO_VALUE_KIND_I8:
            fprintf(out, "%d", value->data.i8);
            break;
        case PLUTO_VALUE_KIND_I16:
            fprintf(out, "%d", value->data.i16);
            break;
        case PLUTO_VALUE_KIND_I32:
            fprintf(out, "%d", value->data.i32);
            break;
        case PLUTO_VALUE_KIND_I64:
            fprintf(out, "%lld", (long long)value->data.i64);
            break;
        case PLUTO_VALUE_KIND_U8:
            fprintf(out, "%u", value->data.u8);
            break;
        case PLUTO_VALUE_KIND_U16:
            fprintf(out, "%u", value->data.u16);
            break;
        case PLUTO_VALUE_KIND_U32:
            fprintf(out, "%u", value->data.u32);
            break;
        case PLUTO_VALUE_KIND_U64:
            fprintf(out, "%llu", (unsigned long long)value->data.u64);
            break;
        case PLUTO_VALUE_KIND_F32:
            fprintf(out, "%f", value->data.f32);
            break;
        case PLUTO_VALUE_KIND_F64:
            fprintf(out, "%lf", value->data.f64);
            break;
        case PLUTO_VALUE_KIND_STRING: {
            pluto_StringView sv = pluto_context_get_string(ctx, value->data.string_tag);
            fprintf(out, "\"%.*s\"", (int)sv.length, sv.data);
            break;
        }
        case PLUTO_VALUE_KIND_TUPLE:
            fprintf(out, "(");
            for (size_t i = 0; i < value->data.tuple.length; ++i) {
                if (i > 0) { fprintf(out, ", "); }
                pluto_value_print(out, value->data.tuple.values[i], ctx);
            }
            fprintf(out, ")");
            break;
        case PLUTO_VALUE_KIND_ARRAY:
            fprintf(out, "[");
            for (size_t i = 0; i < value->data.array.length; ++i) {
                if (i > 0) { fprintf(out, ", "); }
                pluto_value_print(out, value->data.array.values[i], ctx);
            }
            fprintf(out, "]");
            break;
    }
}
