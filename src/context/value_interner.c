// SPDX-License-Identifier: GPL-3.0-or-later

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "context/value_interner.h"

struct _pluto_ValueInterner {
    pluto_Value *buffer;
    size_t length;
    size_t capacity;
};

pluto_ValueInterner pluto_value_interner_construct() {
    return calloc(1, sizeof(struct _pluto_ValueInterner));
}

void pluto_value_interner_destruct(pluto_ValueInterner *vi) {
    if (vi && *vi) {
        for (size_t i = 0; i < (*vi)->length; ++i) {
            pluto_value_destruct(&(*vi)->buffer[i]);
        }
        free((*vi)->buffer);
        free(*vi);
        *vi = NULL;
    }
}

static bool _pluto_value_interner_full(pluto_ValueInterner vi) {
    return (vi->length + 1) >= vi->capacity;
}

static void _pluto_value_interner_grow(pluto_ValueInterner vi) {
    size_t new_capacity = vi->capacity == 0 ? 8 : vi->capacity * 2;
    pluto_Value *new_buffer = realloc(vi->buffer, new_capacity * sizeof(pluto_Value));
    if (new_buffer) {
        vi->buffer = new_buffer;
        vi->capacity = new_capacity;
    }
}

uint32_t pluto_value_interner_intern(pluto_ValueInterner vi, pluto_Value value) {
    if (_pluto_value_interner_full(vi)) {
        _pluto_value_interner_grow(vi);
    }

    for (size_t i = 0; i < vi->length; ++i) {
        pluto_Value v = vi->buffer[i];
        if (pluto_value_kind(v) == pluto_value_kind(value)) {
            if (pluto_value_eq(v, value)) {
                pluto_value_destruct(&value);
                return i;
            }
        }
    }

    vi->buffer[vi->length] = value;
    return vi->length++;
}

pluto_Value pluto_value_interner_at(pluto_ValueInterner vi, uint32_t tag) {
    assert(tag < vi->length);
    return vi->buffer[tag];
}