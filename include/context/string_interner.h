// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef PLUTO_CONTEXT_STRING_INTERNER_H
#define PLUTO_CONTEXT_STRING_INTERNER_H

#include "support/string_view.h"

struct _pluto_StringInterner;
typedef struct _pluto_StringInterner *pluto_StringInterner;

pluto_StringInterner pluto_string_interner_construct();
void pluto_string_interner_destruct(pluto_StringInterner *);

uint32_t pluto_string_interner_intern(pluto_StringInterner, pluto_StringView);
pluto_StringView pluto_string_interner_at(pluto_StringInterner, uint32_t);

#endif // !PLUTO_CONTEXT_STRING_INTERNER_H

