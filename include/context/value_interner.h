// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef PLUTO_TIIR_VALUE_INTERNER_H
#define PLUTO_TIIR_VALUE_INTERNER_H

#include "tiir/value.h"

struct _pluto_ValueInterner;
typedef struct _pluto_ValueInterner *pluto_ValueInterner;

pluto_ValueInterner pluto_value_interner_construct();
void pluto_value_interner_destruct(pluto_ValueInterner *);

uint32_t pluto_value_interner_intern(pluto_ValueInterner, pluto_Value);
pluto_Value pluto_value_interner_at(pluto_ValueInterner, uint32_t);

#endif // !PLUTO_TIIR_VALUE_INTERNER_H
