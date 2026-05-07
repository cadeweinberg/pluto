
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <stdarg.h>

#include "context/string_interner.h"

#include <cmocka.h>

void test_string_interner_intern(void **) {
  pluto_StringInterner si = pluto_string_interner_construct();
  uint32_t tag1 = pluto_string_interner_intern(si, PLUTO_SV("hello"));
  uint32_t tag2 = pluto_string_interner_intern(si, PLUTO_SV("hello"));
  uint32_t tag3 = pluto_string_interner_intern(si, PLUTO_SV("world"));
  assert_int_equal(tag1, tag2);
  assert_int_not_equal(tag1, tag3);
  pluto_StringView sv0 = pluto_string_interner_at(si, tag1);
  pluto_StringView sv1 = pluto_string_interner_at(si, tag2);
  pluto_StringView sv2 = pluto_string_interner_at(si, tag3);
  assert_int_equal(sv0.length, sv1.length);
  assert_memory_equal(sv0.data, sv1.data, sv0.length);
  assert_int_equal(sv2.length, 5);
  assert_memory_equal(sv2.data, "world", sv2.length);

  pluto_string_interner_destruct(&si);
}

int main (void) {
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_string_interner_intern),
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}
