// SPDX-License-Identifier: GPL-3.0-or-later

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <string.h>

#include <cmocka.h>

#include "support/string_view.h"
#include "support/string_constant.h"

void test_string_view_construct(void **) {
  pluto_StringView view = pluto_string_view_construct("Hello, View!", sizeof("Hello, View!") - 1);
  assert_string_equal(view.data, "Hello, View!");
  assert_int_equal(view.length, sizeof("Hello, View!") - 1);
}

void test_string_view_from_cstr(void **) {
  pluto_StringView view = pluto_string_view_from_cstr("Goodbye, View!");
  assert_string_equal(view.data, "Goodbye, View!");
  assert_int_equal(view.length, strlen("Goodbye, View!"));
}

void test_string_view_from_strc(void **) {
  pluto_StringConstant strc = PLUTO_STRC("Heebie Jeebie!");
  pluto_StringView view     = pluto_string_view_from_strc(strc);
  char const *c0 = pluto_string_constant_cstr(strc);
  size_t      l0 = pluto_string_constant_length(strc);
  char const *c1 = pluto_string_view_cstr(view);
  size_t      l1 = pluto_string_view_length(view);
  assert_string_equal(c0, c1);
  assert_int_equal(l0, l1);

  pluto_string_constant_destruct(&strc);
}

void test_string_view_to_strc(void **) {
  pluto_StringView sv = PLUTO_SV("Jinkies!~");
  pluto_StringConstant sc = pluto_string_view_to_strc(sv);

  assert_true(pluto_string_view_cmp_strc(sv, sc) == 0);

  pluto_string_constant_destruct(&sc);
}

void test_string_view_length(void **) {
  char const *cstr = "Nice View!";
  pluto_StringView view = pluto_string_view_from_cstr(cstr);
  assert_int_equal(pluto_string_view_length(view), strlen(cstr));
}

void test_string_view_cstr(void **) {
  char const *cstr = "Beautiful!";
  pluto_StringView view = pluto_string_view_from_cstr(cstr);
  assert_ptr_equal(view.data, pluto_string_view_cstr(view));
}

void test_string_view_cmp(void **) {
  char const *cstr0 = "Amazing!";
  char const *cstr1 = "Bountiful!";
  pluto_StringView view0 = pluto_string_view_from_cstr(cstr0);
  pluto_StringView view1 = pluto_string_view_from_cstr(cstr1);

  assert_true(pluto_string_view_cmp(view0, view1) < 0);
  assert_true(pluto_string_view_cmp(view1, view0) > 0);
  assert_true(pluto_string_view_cmp(view0, view0) == 0);
}

void test_string_view_cmp_cstr(void **) {
  char const *cstr0 = "Amazing!";
  char const *cstr1 = "Bountiful!";
  pluto_StringView view0 = pluto_string_view_from_cstr(cstr0);
  pluto_StringView view1 = pluto_string_view_from_cstr(cstr1);

  assert_true(pluto_string_view_cmp_cstr(view0, cstr1) < 0);
  assert_true(pluto_string_view_cmp_cstr(view1, cstr0) > 0);
  assert_true(pluto_string_view_cmp_cstr(view0, cstr0) == 0);
}

void test_string_view_cmp_strc(void **) {
  char const *c0 = "Wonderful!";
  char const *c1 = "Spectacle!";
  pluto_StringView sv0 = pluto_string_view_from_cstr(c0);
  pluto_StringView sv1 = pluto_string_view_from_cstr(c1);
  pluto_StringConstant sc0 = pluto_string_constant_from_cstr(c0);
  pluto_StringConstant sc1 = pluto_string_constant_from_cstr(c1);

  assert_true(pluto_string_view_cmp_strc(sv0, sc0) == 0);
  assert_true(pluto_string_view_cmp_strc(sv1, sc1) == 0);
  assert_false(pluto_string_view_cmp_strc(sv0, sc1) == 0);
  assert_false(pluto_string_view_cmp_strc(sv1, sc0) == 0);

  pluto_string_constant_destruct(&sc0);
  pluto_string_constant_destruct(&sc1);
}

void test_string_view_macro_PLUTO_SV(void **) {
  pluto_StringView view0 = PLUTO_SV("Easy");
  pluto_StringView view1 = PLUTO_SV("Peasy");

  assert_true(pluto_string_view_cmp(view0, view0) == 0);
  assert_false(pluto_string_view_cmp(view0, view1) == 0);
  assert_int_equal(view0.length, strlen("Easy"));
  assert_string_equal(view0.data, "Easy");
}

int32_t main(int32_t argc, char const *argv[]) {
  const struct CMUnitTest string_view_tests[] = {
    cmocka_unit_test(test_string_view_construct),
    cmocka_unit_test(test_string_view_from_cstr),
    cmocka_unit_test(test_string_view_from_strc),
    cmocka_unit_test(test_string_view_to_strc),
    cmocka_unit_test(test_string_view_length),
    cmocka_unit_test(test_string_view_cstr),
    cmocka_unit_test(test_string_view_cmp),
    cmocka_unit_test(test_string_view_cmp_cstr),
    cmocka_unit_test(test_string_view_cmp_strc),
    cmocka_unit_test(test_string_view_macro_PLUTO_SV),
  };
  return cmocka_run_group_tests(string_view_tests, NULL, NULL);
}
