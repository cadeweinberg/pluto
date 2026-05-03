// SPDX-License-Identifier: GPL-3.0-or-later

#include "support/string_constant.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct _pluto_StringConstant {
  size_t length;
  char data[];
};

pluto_StringConstant pluto_string_constant_construct(char const *data, size_t length) {
  assert(data != NULL);
  assert(strlen(data) == length);
  struct _pluto_StringConstant *strc = malloc(sizeof(pluto_StringConstant) + length + 1);
  strc->length = length;
  memcpy(strc->data, data, length);
  strc->data[length] = '\0';
  return strc;
}

void pluto_string_constant_destruct(pluto_StringConstant *strc) {
  assert(strc != NULL);
  assert(*strc != NULL);
  free(*((void **)strc));
  *strc = NULL;
}

pluto_StringConstant pluto_string_constant_from_cstr(const char *cstr) {
  assert(cstr != NULL);
  size_t length = strlen(cstr);
  return pluto_string_constant_construct(cstr, length);
}

pluto_StringConstant pluto_string_constant_from_view(pluto_StringView view) {
  assert(view.data != NULL);
  return pluto_string_constant_construct(view.data, view.length);
}

pluto_StringView pluto_string_constant_to_view(pluto_StringConstant strc) {
  assert(strc != NULL);
  return pluto_string_view_construct(strc->data, strc->length);
}

size_t pluto_string_constant_length(pluto_StringConstant strc) {
  assert(strc != NULL);
  return strc->length;
}

char const *pluto_string_constant_cstr(pluto_StringConstant strc) {
  assert(strc != NULL);
  return strc->data;
}

int32_t pluto_string_constant_cmp(pluto_StringConstant a, pluto_StringConstant b) {
  assert(a != NULL);
  assert(b != NULL);
  size_t min = a->length < b->length ? a->length : b->length;
  return strncmp(a->data, b->data, min);
}

int32_t pluto_string_constant_cmp_cstr(pluto_StringConstant strc, char const *cstr) {
  assert(strc != NULL);
  assert(cstr != NULL);
  size_t length = strlen(cstr);
  size_t min    = strc->length < length ? strc->length : length;
  return strncmp(strc->data, cstr, min);
}

int32_t pluto_string_constant_cmp_view(pluto_StringConstant strc, pluto_StringView view) {
  assert(strc != NULL);
  assert(view.data != NULL);
  size_t min = strc->length < view.length ? strc->length : view.length;
  return strncmp(strc->data, view.data, min);
}



