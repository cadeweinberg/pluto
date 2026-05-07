// SPDX-License-Identifier: GPL-3.0-or-later

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "context/context.h"
#include "context/string_interner.h"
#include "context/value_interner.h"


struct _pluto_Context {
  pluto_StringInterner _string_interner;
  pluto_ValueInterner _value_interner;
};

pluto_Context pluto_context_construct() {
  pluto_Context ctx = calloc(1, sizeof(struct _pluto_Context));
  ctx->_string_interner = pluto_string_interner_construct();
  ctx->_value_interner = pluto_value_interner_construct();
  return ctx;
}

void pluto_context_destruct(pluto_Context *ctx) {
  if (ctx && *ctx) {
    pluto_string_interner_destruct(&(*ctx)->_string_interner);
    pluto_value_interner_destruct(&(*ctx)->_value_interner);
    free(*ctx);
    *ctx = NULL;
  }
}

uint32_t pluto_context_intern_string(pluto_Context ctx, pluto_StringView sv) {
  assert(ctx);
  return pluto_string_interner_intern(ctx->_string_interner, sv);
}

pluto_StringView pluto_context_get_string(pluto_Context ctx, uint32_t tag) {
  assert(ctx);
  return pluto_string_interner_at(ctx->_string_interner, tag);
}

uint32_t pluto_context_intern_value(pluto_Context ctx, pluto_Value value) {
  assert(ctx);
  return pluto_value_interner_intern(ctx->_value_interner, value);
}

pluto_Value pluto_context_get_value(pluto_Context ctx, uint32_t tag) {
  assert(ctx);
  return pluto_value_interner_at(ctx->_value_interner, tag);
}
