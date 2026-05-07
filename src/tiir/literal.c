// SPDX-License-Identifier: GPL-3.0-or-later

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "tiir/literal.h"
#include "context/context.h"

struct _pluto_LiteralTuple {
    pluto_Literal *values;
    size_t length;
    size_t capacity;
};

struct _pluto_LiteralArray {
    pluto_Literal *values;
    size_t length;
    size_t capacity;
};

struct _pluto_Literal {
    pluto_LiteralKind kind;
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
        struct _pluto_LiteralTuple tuple;
        struct _pluto_LiteralArray array;
    } data;
};

pluto_Literal _pluto_literal_construct() {
    return calloc(1, sizeof(struct _pluto_Literal));
}

void _pluto_literal_destruct_data(pluto_Literal *value) {
	switch ((*value)->kind) {
	case PLUTO_LITERAL_KIND_TUPLE:
		for (size_t i = 0; i < (*value)->data.tuple.length; ++i) {
			pluto_literal_destruct(&(*value)->data.tuple.values[i]);
		}
		free((*value)->data.tuple.values);
		break;
	case PLUTO_LITERAL_KIND_ARRAY:
		for (size_t i = 0; i < (*value)->data.array.length; ++i) {
			pluto_literal_destruct(&(*value)->data.array.values[i]);
		}
		free((*value)->data.array.values);
		break;
	default:
		break;
}
}

void pluto_literal_destruct(pluto_Literal *value) {
    if (value && *value) {
        _pluto_literal_destruct_data(value);
        free(*value);
        *value = NULL;
    }
}

pluto_Literal pluto_literal_construct_nil() {
    pluto_Literal value = _pluto_literal_construct();
    value->kind = PLUTO_LITERAL_KIND_NIL;
    return value;
}

pluto_Literal pluto_literal_construct_bool(bool b) {
    pluto_Literal value = _pluto_literal_construct();
    value->kind = PLUTO_LITERAL_KIND_BOOL;
    value->data.b = b;
    return value;
}

pluto_Literal pluto_literal_construct_i8(int8_t i) {
    pluto_Literal value = _pluto_literal_construct();
    value->kind = PLUTO_LITERAL_KIND_I8;
    value->data.i8 = i;
    return value;
}

pluto_Literal pluto_literal_construct_i16(int16_t i) {
    pluto_Literal value = _pluto_literal_construct();
    value->kind = PLUTO_LITERAL_KIND_I16;
    value->data.i16 = i;
    return value;
}

pluto_Literal pluto_literal_construct_i32(int32_t i) {
    pluto_Literal value = _pluto_literal_construct();
    value->kind = PLUTO_LITERAL_KIND_I32;
    value->data.i32 = i;
    return value;
}

pluto_Literal pluto_literal_construct_i64(int64_t i) {
    pluto_Literal value = _pluto_literal_construct();
    value->kind = PLUTO_LITERAL_KIND_I64;
    value->data.i64 = i;
    return value;
}

pluto_Literal pluto_literal_construct_u8(uint8_t u) {
    pluto_Literal value = _pluto_literal_construct();
    value->kind = PLUTO_LITERAL_KIND_U8;
    value->data.u8 = u;
    return value;
}

pluto_Literal pluto_literal_construct_u16(uint16_t u) {
    pluto_Literal value = _pluto_literal_construct();
    value->kind = PLUTO_LITERAL_KIND_U16;
    value->data.u16 = u;
    return value;
}

pluto_Literal pluto_literal_construct_u32(uint32_t u) {
    pluto_Literal value = _pluto_literal_construct();
    value->kind = PLUTO_LITERAL_KIND_U32;
    value->data.u32 = u;
    return value;
}

pluto_Literal pluto_literal_construct_u64(uint64_t u) {
    pluto_Literal value = _pluto_literal_construct();
    value->kind = PLUTO_LITERAL_KIND_U64;
    value->data.u64 = u;
    return value;
}

pluto_Literal pluto_literal_construct_f32(float f) {
    pluto_Literal value = _pluto_literal_construct();
    value->kind = PLUTO_LITERAL_KIND_F32;
    value->data.f32 = f;
    return value;
}

pluto_Literal pluto_literal_construct_f64(double f) {
    pluto_Literal value = _pluto_literal_construct();
    value->kind = PLUTO_LITERAL_KIND_F64;
    value->data.f64 = f;
    return value;
}

pluto_Literal pluto_literal_construct_string(uint32_t string_tag) {
    pluto_Literal value = _pluto_literal_construct();
    value->kind = PLUTO_LITERAL_KIND_STRING;
    value->data.string_tag = string_tag;
    return value;
}

pluto_Literal pluto_literal_construct_tuple() {
    pluto_Literal value = _pluto_literal_construct();
    value->kind = PLUTO_LITERAL_KIND_TUPLE;
    value->data.tuple.values = NULL;
    value->data.tuple.length = 0;
    value->data.tuple.capacity = 0;
    return value;
}

pluto_Literal pluto_literal_construct_array() {
    pluto_Literal value = _pluto_literal_construct();
    value->kind = PLUTO_LITERAL_KIND_ARRAY;
    value->data.array.values = NULL;
    value->data.array.length = 0;
    value->data.array.capacity = 0;
    return value;
}

pluto_LiteralKind pluto_literal_kind(pluto_Literal value) {
    assert(value != NULL);
    return value->kind;
}

bool pluto_literal_is(pluto_Literal value, pluto_LiteralKind kind) {
    assert(value != NULL);
    return value->kind == kind;
}

bool pluto_literal_get_bool(pluto_Literal value) {
    assert(pluto_literal_is(value, PLUTO_LITERAL_KIND_BOOL));
    return value->data.b;
}

int8_t pluto_literal_get_i8(pluto_Literal value) {
    assert(pluto_literal_is(value, PLUTO_LITERAL_KIND_I8));
    return value->data.i8;
}

int16_t pluto_literal_get_i16(pluto_Literal value) {
    assert(pluto_literal_is(value, PLUTO_LITERAL_KIND_I16));
    return value->data.i16;
}

int32_t pluto_literal_get_i32(pluto_Literal value) {
    assert(pluto_literal_is(value, PLUTO_LITERAL_KIND_I32));
    return value->data.i32;
}

int64_t pluto_literal_get_i64(pluto_Literal value) {
    assert(pluto_literal_is(value, PLUTO_LITERAL_KIND_I64));
    return value->data.i64;
}

uint8_t pluto_literal_get_u8(pluto_Literal value) {
    assert(pluto_literal_is(value, PLUTO_LITERAL_KIND_U8));
    return value->data.u8;
}

uint16_t pluto_literal_get_u16(pluto_Literal value) {
    assert(pluto_literal_is(value, PLUTO_LITERAL_KIND_U16));
    return value->data.u16;
}

uint32_t pluto_literal_get_u32(pluto_Literal value) {
    assert(pluto_literal_is(value, PLUTO_LITERAL_KIND_U32));
    return value->data.u32;
}

uint64_t pluto_literal_get_u64(pluto_Literal value) {
    assert(pluto_literal_is(value, PLUTO_LITERAL_KIND_U64));
    return value->data.u64;
}

float pluto_literal_get_f32(pluto_Literal value) {
    assert(pluto_literal_is(value, PLUTO_LITERAL_KIND_F32));
    return value->data.f32;
}

double pluto_literal_get_f64(pluto_Literal value) {
    assert(pluto_literal_is(value, PLUTO_LITERAL_KIND_F64));
    return value->data.f64;
}

uint32_t pluto_literal_get_string_tag(pluto_Literal value) {
    assert(pluto_literal_is(value, PLUTO_LITERAL_KIND_STRING));
    return value->data.string_tag;
}

pluto_LiteralTuple pluto_literal_get_tuple(pluto_Literal value) {
    assert(pluto_literal_is(value, PLUTO_LITERAL_KIND_TUPLE));
    return &value->data.tuple;
}

pluto_LiteralArray pluto_literal_get_array(pluto_Literal value) {
    assert(pluto_literal_is(value, PLUTO_LITERAL_KIND_ARRAY));
    return &value->data.array;
}

static bool _pluto_literal_tuple_full(pluto_LiteralTuple tuple) {
    return tuple->length >= tuple->capacity;
}

static void _pluto_literal_tuple_grow(pluto_LiteralTuple tuple) {
    size_t new_capacity = tuple->capacity == 0 ? 4 : tuple->capacity * 2;
    pluto_Literal *new_values = realloc(tuple->values, new_capacity * sizeof(pluto_Literal));
    if (new_values) {
        tuple->values = new_values;
        tuple->capacity = new_capacity;
    }
}

void pluto_literal_tuple_append(pluto_LiteralTuple tuple, pluto_Literal value) {
    if (_pluto_literal_tuple_full(tuple)) {
        _pluto_literal_tuple_grow(tuple);
    }
    tuple->values[tuple->length++] = value;
}

pluto_Literal pluto_literal_tuple_at(pluto_LiteralTuple tuple, size_t index) {
    assert(index < tuple->length);
    return tuple->values[index];
}

size_t pluto_literal_tuple_length(pluto_LiteralTuple tuple) {
    return tuple->length;
}

static bool _pluto_literal_array_full(pluto_LiteralArray array) {
    return array->length >= array->capacity;
}

static void _pluto_literal_array_grow(pluto_LiteralArray array) {
    size_t new_capacity = array->capacity == 0 ? 4 : array->capacity * 2;
    pluto_Literal *new_values = realloc(array->values, new_capacity * sizeof(pluto_Literal));
    if (new_values) {
        array->values = new_values;
        array->capacity = new_capacity;
    }
}

void pluto_literal_array_append(pluto_LiteralArray array, pluto_Literal value) {
    if (_pluto_literal_array_full(array)) {
        _pluto_literal_array_grow(array);
    }
    array->values[array->length++] = value;
}

pluto_Literal pluto_literal_array_at(pluto_LiteralArray array, size_t index) {
    assert(index < array->length);
    return array->values[index];
}

size_t pluto_literal_array_length(pluto_LiteralArray array) {
    return array->length;
}

static bool _pluto_literal_tuple_eq(pluto_LiteralTuple a, pluto_LiteralTuple b) {
	if (a->length != b->length) { return false; }
	for (size_t i = 0; i < a->length; ++i) {
		if (!pluto_literal_eq(a->values[i], b->values[i])) {
			return false;
		}
	}
	return true;
}

static bool _pluto_literal_array_eq(pluto_LiteralArray a, pluto_LiteralArray b) {
	if (a->length != b->length) { return false; }
	for (size_t i = 0; i < a->length; ++i) {
		if (!pluto_literal_eq(a->values[i], b->values[i])) {
			return false;
		}
	}
	return true;
}

bool pluto_literal_eq(pluto_Literal a, pluto_Literal b) {
    if (a->kind != b->kind) { return false; }
    switch (a->kind) {
        case PLUTO_LITERAL_KIND_NIL:
            return true;
        case PLUTO_LITERAL_KIND_BOOL:
            return a->data.b == b->data.b;
        case PLUTO_LITERAL_KIND_I8:
            return a->data.i8 == b->data.i8;
        case PLUTO_LITERAL_KIND_I16:
            return a->data.i16 == b->data.i16;
        case PLUTO_LITERAL_KIND_I32:
            return a->data.i32 == b->data.i32;
        case PLUTO_LITERAL_KIND_I64:
            return a->data.i64 == b->data.i64;
        case PLUTO_LITERAL_KIND_U8:
            return a->data.u8 == b->data.u8;
        case PLUTO_LITERAL_KIND_U16:
            return a->data.u16 == b->data.u16;
        case PLUTO_LITERAL_KIND_U32:
            return a->data.u32 == b->data.u32;
        case PLUTO_LITERAL_KIND_U64:
            return a->data.u64 == b->data.u64;
        case PLUTO_LITERAL_KIND_F32:
            return a->data.f32 == b->data.f32;
        case PLUTO_LITERAL_KIND_F64:
            return a->data.f64 == b->data.f64;
        case PLUTO_LITERAL_KIND_STRING:
            return a->data.string_tag == b->data.string_tag;
        case PLUTO_LITERAL_KIND_TUPLE: {
            return _pluto_literal_tuple_eq(&a->data.tuple, &b->data.tuple);
        }
        case PLUTO_LITERAL_KIND_ARRAY: {
            return _pluto_literal_array_eq(&a->data.array, &b->data.array);
        }
    }
    return false;
}

static void _pluto_literal_string_print(FILE *out, pluto_Literal value, struct _pluto_Context *ctx) {
    pluto_StringView sv = pluto_context_get_string(ctx, value->data.string_tag);
    fprintf(out, "\"%.*s\"", (int)sv.length, sv.data);
}

static void _pluto_literal_tuple_print(FILE* out, pluto_LiteralTuple tuple,
								struct _pluto_Context *ctx) {
	fprintf(out, "(");
	for (size_t i = 0; i < tuple->length; ++i) {
		if (i > 0) { fprintf(out, ", "); }
		pluto_literal_print(out, tuple->values[i], ctx);
	}
	fprintf(out, ")");									
}

static void _pluto_literal_array_print(FILE *out, pluto_LiteralArray array,
								struct _pluto_Context *ctx) {
	fprintf(out, "[");
	for (size_t i = 0; i < array->length; ++i) {
		if (i > 0) { fprintf(out, ", "); }
		pluto_literal_print(out, array->values[i], ctx);
	}
    fprintf(out, "]");									
}

void pluto_literal_print(FILE *out, pluto_Literal value, struct _pluto_Context *ctx) {
    assert(value != NULL);
    switch (value->kind) {
        case PLUTO_LITERAL_KIND_NIL:
            fprintf(out, "nil");
            break;
        case PLUTO_LITERAL_KIND_BOOL:
            fprintf(out, value->data.b ? "true" : "false");
            break;
        case PLUTO_LITERAL_KIND_I8:
            fprintf(out, "%d", value->data.i8);
            break;
        case PLUTO_LITERAL_KIND_I16:
            fprintf(out, "%d", value->data.i16);
            break;
        case PLUTO_LITERAL_KIND_I32:
            fprintf(out, "%d", value->data.i32);
            break;
        case PLUTO_LITERAL_KIND_I64:
            fprintf(out, "%lld", (long long)value->data.i64);
            break;
        case PLUTO_LITERAL_KIND_U8:
            fprintf(out, "%u", value->data.u8);
            break;
        case PLUTO_LITERAL_KIND_U16:
            fprintf(out, "%u", value->data.u16);
            break;
        case PLUTO_LITERAL_KIND_U32:
            fprintf(out, "%u", value->data.u32);
            break;
        case PLUTO_LITERAL_KIND_U64:
            fprintf(out, "%llu", (unsigned long long)value->data.u64);
            break;
        case PLUTO_LITERAL_KIND_F32:
            fprintf(out, "%f", value->data.f32);
            break;
        case PLUTO_LITERAL_KIND_F64:
            fprintf(out, "%lf", value->data.f64);
            break;
        case PLUTO_LITERAL_KIND_STRING: {
            _pluto_literal_string_print(out, value, ctx);
            break;
        }
        case PLUTO_LITERAL_KIND_TUPLE:
            _pluto_literal_tuple_print(out, &value->data.tuple, ctx);
            break;
        case PLUTO_LITERAL_KIND_ARRAY:
            _pluto_literal_array_print(out, &value->data.array, ctx);
            break;
    }
}
