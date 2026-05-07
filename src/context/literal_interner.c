// SPDX-License-Identifier: GPL-3.0-or-later

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "context/literal_interner.h"

struct _pluto_LiteralInterner {
    pluto_Literal *buffer;
    size_t length;
    size_t capacity;
};

pluto_LiteralInterner pluto_literal_interner_construct() {
    return calloc(1, sizeof(struct _pluto_LiteralInterner));
}

void pluto_literal_interner_destruct(pluto_LiteralInterner *vi) {
    if (vi && *vi) {
        for (size_t i = 0; i < (*vi)->length; ++i) {
            pluto_literal_destruct(&(*vi)->buffer[i]);
        }
        free((*vi)->buffer);
        free(*vi);
        *vi = NULL;
    }
}

static bool _pluto_literal_interner_full(pluto_LiteralInterner vi) {
    return (vi->length + 1) >= vi->capacity;
}

static void _pluto_literal_interner_grow(pluto_LiteralInterner vi) {
    size_t new_capacity = vi->capacity == 0 ? 8 : vi->capacity * 2;
    pluto_Literal *new_buffer = realloc(vi->buffer, new_capacity * sizeof(pluto_Literal));
    if (new_buffer) {
        vi->buffer = new_buffer;
        vi->capacity = new_capacity;
    }
}

uint32_t pluto_literal_interner_intern(pluto_LiteralInterner vi, pluto_Literal value) {
    if (_pluto_literal_interner_full(vi)) {
        _pluto_literal_interner_grow(vi);
    }

    for (size_t i = 0; i < vi->length; ++i) {
        pluto_Literal v = vi->buffer[i];
        if (pluto_literal_kind(v) == pluto_literal_kind(value)) {
            if (pluto_literal_eq(v, value)) {
                pluto_literal_destruct(&value);
                return i;
            }
        }
    }

    vi->buffer[vi->length] = value;
    return vi->length++;
}

pluto_Literal pluto_literal_interner_at(pluto_LiteralInterner vi, uint32_t tag) {
    assert(tag < vi->length);
    return vi->buffer[tag];
}