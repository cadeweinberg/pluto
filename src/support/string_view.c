// SPDX-License-Identifier: GPL-3.0-or-later

#include <assert.h>
#include <string.h>

#include "support/string_view.h"
#include "support/string_constant.h"

pluto_StringView pluto_string_view_construct(char const *data, size_t length) {
  return (pluto_StringView){.data = data, .length = length};
}

pluto_StringView pluto_string_view_from_cstr(char const *cstr) {
  assert(cstr);
  return (pluto_StringView){.data = cstr, .length = strlen(cstr)};
}

pluto_StringView pluto_string_view_from_strc(pluto_StringConstant strc) {
  assert(strc);
  size_t    length = pluto_string_constant_length(strc);
  char const *cstr = pluto_string_constant_cstr(strc);
  return pluto_string_view_construct(cstr, length);
}

pluto_StringConstant pluto_string_view_to_strc(pluto_StringView view) {
  assert(view.data);
  return pluto_string_constant_construct(view.data, view.length);
}

size_t pluto_string_view_length(pluto_StringView view) { return view.length; }

char const *pluto_string_view_cstr(pluto_StringView view) {
  assert(view.data);
  return view.data;
}

int32_t pluto_string_view_cmp(pluto_StringView a, pluto_StringView b) {
  size_t min = a.length < b.length ? a.length : b.length;
  return strncmp(a.data, b.data, min);
}

int32_t pluto_string_view_cmp_cstr(pluto_StringView view, char const *cstr) {
  assert(cstr);
  size_t length = strlen(cstr);
  size_t    min = view.length < length ? view.length : length;
  return strncmp(view.data, cstr, min);
}

int32_t pluto_string_view_cmp_strc(pluto_StringView view, pluto_StringConstant strc) {
  assert(strc);
  char const * cstr = pluto_string_constant_cstr(strc);
  size_t     length = pluto_string_constant_length(strc);
  size_t        min = view.length < length ? view.length : length;
  return strncmp(view.data, cstr, min);
}

