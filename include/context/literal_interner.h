// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef PLUTO_TIIR_LITERAL_INTERNER_H
#define PLUTO_TIIR_LITERAL_INTERNER_H

#include "tiir/literal.h"

struct _pluto_LiteralInterner;
typedef struct _pluto_LiteralInterner *pluto_LiteralInterner;

pluto_LiteralInterner pluto_literal_interner_construct();
void pluto_literal_interner_destruct(pluto_LiteralInterner *);

uint32_t pluto_literal_interner_intern(pluto_LiteralInterner, pluto_Literal);
pluto_Literal pluto_literal_interner_at(pluto_LiteralInterner, uint32_t);

#endif // !PLUTO_TIIR_LITERAL_INTERNER_H
