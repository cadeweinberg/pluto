// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef PLUTO_SUPPORT_pluto_string_view_H
#define PLUTO_SUPPORT_pluto_string_view_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// .. c:struct:: pluto_StringView
//    A structure representing a view into a string.
//    c:member char const *data: A pointer to the string data.
//    c:member size_t length: The length of the string view.
typedef struct pluto_StringView {
  char const *data;
  size_t length;
} pluto_StringView;

pluto_StringView pluto_string_view();
pluto_StringView pluto_string_view_create(char const *data, size_t length);
pluto_StringView pluto_string_view_from_cstr(char const *cstr);
size_t pluto_string_view_length(pluto_StringView view);
char const *pluto_string_view_cstr(pluto_StringView view);
int32_t pluto_string_view_cmp(pluto_StringView a, pluto_StringView b);
int32_t pluto_string_view_cmp_cstr(pluto_StringView view, char const *cstr);
bool pluto_string_view_eq(pluto_StringView a, pluto_StringView b);
bool pluto_string_view_eq_cstr(pluto_StringView view, char const *cstr);

// .. c:macro:: SV
//    A macro to create a pluto_StringView from a string literal.
//    c:param str: The string literal to create the pluto_StringView from.
#define SV(str) ((pluto_StringView){.data = (str), .length = sizeof(str) - 1})

#endif // !PLUTO_SUPPORT_pluto_string_view_H
