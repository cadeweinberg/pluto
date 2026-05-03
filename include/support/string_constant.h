// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef PLUTO_SUPPORT_STRING_CONSTANT_H
#define PLUTO_SUPPORT_STRING_CONSTANT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "support/string_view.h"

struct _pluto_StringConstant;
typedef struct _pluto_StringConstant const *pluto_StringConstant;

pluto_StringConstant pluto_string_constant_construct(char const *data, size_t length);
void pluto_string_constant_destruct(pluto_StringConstant* strc);

pluto_StringConstant pluto_string_constant_from_cstr(const char *cstr);
pluto_StringConstant pluto_string_constant_from_view(pluto_StringView view);
pluto_StringView pluto_string_constant_to_view(pluto_StringConstant strc);

size_t pluto_string_constant_length(pluto_StringConstant strc);
char const *pluto_string_constant_cstr(pluto_StringConstant strc);

int32_t pluto_string_constant_cmp(pluto_StringConstant a, pluto_StringConstant b);
int32_t pluto_string_constant_cmp_cstr(pluto_StringConstant strc, char const *cstr);
int32_t pluto_string_constant_cmp_view(pluto_StringConstant strc, pluto_StringView view);

#define PLUTO_STRC(str) (pluto_string_constant_from_cstr(str))

#endif // !PLUTO_SUPPORT_STRING_CONSTANT_H


