// SPDX-License-Identifier: GPL-3.0-or-later

/*!
 * \file support/string_view.h
 * \brief declare the pluto_StringView
 */

#ifndef PLUTO_SUPPORT_STRING_VIEW_H
#define PLUTO_SUPPORT_STRING_VIEW_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct _pluto_StringConstant;

typedef struct _pluto_StringView {
  char const *data;
  size_t length;
} pluto_StringView;

pluto_StringView pluto_string_view_construct(char const *data, size_t length);
pluto_StringView pluto_string_view_from_cstr(char const *cstr);
pluto_StringView pluto_string_view_from_strc(struct _pluto_StringConstant const *strc);
struct _pluto_StringConstant const *pluto_string_view_to_strc(pluto_StringView view);
size_t pluto_string_view_length(pluto_StringView view);
char const *pluto_string_view_cstr(pluto_StringView view);
int32_t pluto_string_view_cmp(pluto_StringView a, pluto_StringView b);
int32_t pluto_string_view_cmp_cstr(pluto_StringView view, char const *cstr);
int32_t pluto_string_view_cmp_strc(pluto_StringView view, struct _pluto_StringConstant const *strc);

#define PLUTO_SV(str) ((pluto_StringView){.data = (str), .length = sizeof(str) - 1})

#endif // !PLUTO_SUPPORT_STRING_VIEW_H
