// SPDX-License-Identifier: GPL-3.0-or-later

#include <assert.h>
#include <string.h>

#include "support/string_view.h"

pluto_StringView pluto_string_view() { return (pluto_StringView){.data = NULL, .length = 0}; }

pluto_StringView pluto_string_view_create(char const *data, size_t length) {
  return (pluto_StringView){.data = data, .length = length};
}

pluto_StringView pluto_string_view_from_cstr(char const *cstr) {
  assert(cstr != NULL);
  return (pluto_StringView){.data = cstr, .length = strlen(cstr)};
}

size_t pluto_string_view_length(pluto_StringView view) { return view.length; }

char const *pluto_string_view_cstr(pluto_StringView view) {
  assert(view.data != NULL);
  return view.data;
}

int32_t pluto_string_view_cmp(pluto_StringView a, pluto_StringView b) {
  size_t min_length = a.length < b.length ? a.length : b.length;
  return strncmp(a.data, b.data, min_length);
}

int32_t pluto_string_view_cmp_cstr(pluto_StringView view, char const *cstr) {
  assert(cstr != NULL);
  size_t cstr_length = strlen(cstr);
  size_t min_length = view.length < cstr_length ? view.length : cstr_length;
  return strncmp(view.data, cstr, min_length);
}

bool pluto_string_view_eq(pluto_StringView a, pluto_StringView b) {
  if (a.length != b.length) {
    return false;
  }
  return strncmp(a.data, b.data, a.length) == 0;
}

bool pluto_string_view_eq_cstr(pluto_StringView view, char const *cstr) {
  assert(cstr != NULL);
  size_t cstr_length = strlen(cstr);
  if (view.length != cstr_length) {
    return false;
  }
  return strncmp(view.data, cstr, view.length) == 0;
}
