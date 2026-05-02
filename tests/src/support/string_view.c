// SPDX-License-Identifier: GPL-3.0-or-later

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <string.h>

#include <cmocka.h>

#include "support/string_view.h"

void test_string_view(void **) {
  pluto_StringView view = pluto_string_view();
  assert_ptr_equal(view.data, NULL);
  assert_int_equal(view.length, 0);
}

void test_string_view_create(void **) {
  pluto_StringView view = pluto_string_view_create("Hello, View!", sizeof("Hello, View!") - 1);
  assert_string_equal(view.data, "Hello, View!");
  assert_int_equal(view.length, sizeof("Hello, View!") - 1);
}

void test_string_view_from_cstr(void **) {
  pluto_StringView view = pluto_string_view_from_cstr("Goodbye, View!");
  assert_string_equal(view.data, "Goodbye, View!");
  assert_int_equal(view.length, strlen("Goodbye, View!"));
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

void test_string_view_eq(void **) {
  pluto_StringView view0 = pluto_string_view_from_cstr("Hello");
  pluto_StringView view1 = pluto_string_view_from_cstr("Trouble");

  assert_true(pluto_string_view_eq(view0, view0));
  assert_false(pluto_string_view_eq(view1, view0));
}

void test_string_view_eq_cstr(void **) {
  char const *cstr0 = "Gargantuan";
  pluto_StringView view0 = pluto_string_view_from_cstr(cstr0);
  pluto_StringView view1 = pluto_string_view_from_cstr("Miniscule");

  assert_true(pluto_string_view_eq_cstr(view0, cstr0));
  assert_false(pluto_string_view_eq_cstr(view1, cstr0));
}

void test_string_view_macro_SV(void **) {
  pluto_StringView view0 = SV("Easy");
  pluto_StringView view1 = SV("Peasy");

  assert_true(pluto_string_view_eq(view0, view0));
  assert_false(pluto_string_view_eq(view0, view1));
  assert_int_equal(view0.length, strlen("Easy"));
  assert_string_equal(view0.data, "Easy");
}

int32_t main(int32_t argc, char const *argv[]) {
  const struct CMUnitTest string_view_tests[] = {
    cmocka_unit_test(test_string_view),
    cmocka_unit_test(test_string_view_create),
    cmocka_unit_test(test_string_view_from_cstr),
    cmocka_unit_test(test_string_view_length),
    cmocka_unit_test(test_string_view_cstr),
    cmocka_unit_test(test_string_view_cmp),
    cmocka_unit_test(test_string_view_cmp_cstr),
    cmocka_unit_test(test_string_view_eq),
    cmocka_unit_test(test_string_view_eq_cstr),
    cmocka_unit_test(test_string_view_macro_SV),
  };
  return cmocka_run_group_tests(string_view_tests, NULL, NULL);
}
