// SPDX-License-Identifier: GPL-3.0-or-later

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <setjmp.h>
#include <string.h>

#include <cmocka.h>

#include "support/string_constant.h"
#include "support/string_view.h"

void test_string_constant_construct(void **) {
  char const *cs0 = "Jenkins";
  size_t       l0 = strlen(cs0);
  pluto_StringConstant strc = pluto_string_constant_construct(cs0, l0);
  char const *cs1 = pluto_string_constant_cstr(strc);
  size_t       l1 = pluto_string_constant_length(strc);

  assert_string_equal(cs0, cs1);
  assert_int_equal(l0, l1);

  pluto_string_constant_destruct(&strc);
}

void test_string_constant_from_cstr(void **) {
  char const *cs0 = "Hilda";
  size_t       l0 = strlen(cs0);
  pluto_StringConstant sc = pluto_string_constant_from_cstr(cs0);
  char const *cs1 = pluto_string_constant_cstr(sc);
  size_t       l1 = pluto_string_constant_length(sc);

  assert_string_equal(cs0, cs1);
  assert_int_equal(l0, l1);

  pluto_string_constant_destruct(&sc);
}

void test_string_constant_from_view(void **) {
  pluto_StringView     sv = PLUTO_SV("Sunny");
  pluto_StringConstant sc = pluto_string_constant_from_view(sv);
  char const          *cs = pluto_string_constant_cstr(sc);
  size_t                l = pluto_string_constant_length(sc);

  assert_string_equal(cs, sv.data);
  assert_int_equal(l, sv.length);

  pluto_string_constant_destruct(&sc);
}

void test_string_constant_to_view(void **) {
  char const          *cs = "Raul";
  size_t                l = strlen(cs);
  pluto_StringConstant sc = pluto_string_constant_construct(cs, l);
  pluto_StringView     sv = pluto_string_constant_to_view(sc);

  assert_string_equal(cs, sv.data);
  assert_int_equal(l, sv.length);

  pluto_string_constant_destruct(&sc);
}

void test_string_constant_cmp(void **) {
  pluto_StringConstant sc0 = PLUTO_STRC("Benny");
  pluto_StringConstant sc1 = PLUTO_STRC("Dogmeat");

  assert_true(pluto_string_constant_cmp(sc0, sc0) == 0);
  assert_true(pluto_string_constant_cmp(sc0, sc1) < 0);
  assert_true(pluto_string_constant_cmp(sc1, sc0) > 0);
  assert_false(pluto_string_constant_cmp(sc0, sc1) == 0);

  pluto_string_constant_destruct(&sc0);
  pluto_string_constant_destruct(&sc1);
}

void test_string_constant_cmp_cstr(void **) {
  char const          *edith = "Edith";
  char const           *rosa = "Rosa";
  char const          *betty = "Betty";
  pluto_StringConstant cs = PLUTO_STRC(edith);

  assert_true(pluto_string_constant_cmp_cstr(cs, edith) == 0);
  assert_true(pluto_string_constant_cmp_cstr(cs, rosa) < 0);
  assert_true(pluto_string_constant_cmp_cstr(cs, betty) > 0);

  pluto_string_constant_destruct(&cs);
}

void test_string_constant_cmp_view(void **) {
  pluto_StringView albert = PLUTO_SV("Abert");
  pluto_StringView jill   = PLUTO_SV("Jill");
  pluto_StringView chris  = PLUTO_SV("Chris");
  pluto_StringConstant cs = pluto_string_constant_from_view(chris);

  assert_true(pluto_string_constant_cmp_view(cs, chris) == 0);
  assert_true(pluto_string_constant_cmp_view(cs, albert) > 0);
  assert_true(pluto_string_constant_cmp_view(cs, jill) < 0);

  pluto_string_constant_destruct(&cs);
}

int32_t main(void) {
  const struct CMUnitTest string_constant_tests[] = {
    cmocka_unit_test(test_string_constant_construct),
    cmocka_unit_test(test_string_constant_from_cstr),
    cmocka_unit_test(test_string_constant_from_view),
    cmocka_unit_test(test_string_constant_to_view),
    cmocka_unit_test(test_string_constant_cmp),
    cmocka_unit_test(test_string_constant_cmp_cstr),
    cmocka_unit_test(test_string_constant_cmp_view),
  };

  return cmocka_run_group_tests(string_constant_tests, NULL, NULL);
}



