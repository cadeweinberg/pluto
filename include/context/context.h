// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef PLUTO_CONTEXT_CONTEXT_H
#define PLUTO_CONTEXT_CONTEXT_H

#include <stdint.h>

#include "support/string_view.h"
#include "tiir/literal.h"

struct _pluto_Context;
typedef struct _pluto_Context *pluto_Context;

pluto_Context pluto_context_construct();
void pluto_context_destruct(pluto_Context *ctx);

uint32_t pluto_context_intern_string(pluto_Context ctx, pluto_StringView sv);
pluto_StringView pluto_context_get_string(pluto_Context ctx, uint32_t tag);

uint32_t pluto_context_intern_literal(pluto_Context ctx, pluto_Literal literal);
pluto_Literal pluto_context_get_literal(pluto_Context ctx, uint32_t tag);

#endif // !PLUTO_CONTEXT_CONTEXT_H
