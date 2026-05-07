// SPDX-License-Identifier: GPL-3.0-or-later

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>

#include <cmocka.h>

#include "context/context.h"
#include "tiir/literal.h"

void test_literal_construct_nil(void **) {
  pluto_Literal value = pluto_literal_construct_nil();
  assert_true(pluto_literal_is(value, PLUTO_LITERAL_KIND_NIL));
  assert_int_equal(pluto_literal_kind(value), PLUTO_LITERAL_KIND_NIL);
  pluto_literal_destruct(&value);
  assert_null(value);
}

void test_literal_construct_bool(void **) {
  pluto_Literal value_true = pluto_literal_construct_bool(true);
  pluto_Literal value_false = pluto_literal_construct_bool(false);
  assert_true(pluto_literal_is(value_true, PLUTO_LITERAL_KIND_BOOL));
  assert_true(pluto_literal_get_bool(value_true));
  assert_false(pluto_literal_get_bool(value_false));
  pluto_literal_destruct(&value_true);
  pluto_literal_destruct(&value_false);
}

void test_literal_construct_i8(void **) {
  pluto_Literal value = pluto_literal_construct_i8(-12);
  assert_true(pluto_literal_is(value, PLUTO_LITERAL_KIND_I8));
  assert_int_equal(pluto_literal_get_i8(value), -12);
  pluto_literal_destruct(&value);
}

void test_literal_construct_i16(void **) {
  pluto_Literal value = pluto_literal_construct_i16(-1234);
  assert_true(pluto_literal_is(value, PLUTO_LITERAL_KIND_I16));
  assert_int_equal(pluto_literal_get_i16(value), -1234);
  pluto_literal_destruct(&value);
}

void test_literal_construct_i32(void **) {
  pluto_Literal value = pluto_literal_construct_i32(-123456);
  assert_true(pluto_literal_is(value, PLUTO_LITERAL_KIND_I32));
  assert_int_equal(pluto_literal_get_i32(value), -123456);
  pluto_literal_destruct(&value);
}

void test_literal_construct_i64(void **) {
  pluto_Literal value = pluto_literal_construct_i64(-1234567890LL);
  assert_true(pluto_literal_is(value, PLUTO_LITERAL_KIND_I64));
  assert_int_equal(pluto_literal_get_i64(value), -1234567890LL);
  pluto_literal_destruct(&value);
}

void test_literal_construct_u8(void **) {
  pluto_Literal value = pluto_literal_construct_u8(12);
  assert_true(pluto_literal_is(value, PLUTO_LITERAL_KIND_U8));
  assert_int_equal(pluto_literal_get_u8(value), 12);
  pluto_literal_destruct(&value);
}

void test_literal_construct_u16(void **) {
  pluto_Literal value = pluto_literal_construct_u16(1234);
  assert_true(pluto_literal_is(value, PLUTO_LITERAL_KIND_U16));
  assert_int_equal(pluto_literal_get_u16(value), 1234);
  pluto_literal_destruct(&value);
}

void test_literal_construct_u32(void **) {
  pluto_Literal value = pluto_literal_construct_u32(123456U);
  assert_true(pluto_literal_is(value, PLUTO_LITERAL_KIND_U32));
  assert_int_equal(pluto_literal_get_u32(value), 123456U);
  pluto_literal_destruct(&value);
}

void test_literal_construct_u64(void **) {
  pluto_Literal value = pluto_literal_construct_u64(1234567890ULL);
  assert_true(pluto_literal_is(value, PLUTO_LITERAL_KIND_U64));
  assert_int_equal(pluto_literal_get_u64(value), 1234567890ULL);
  pluto_literal_destruct(&value);
}

void test_literal_construct_f32(void **) {
  pluto_Literal value = pluto_literal_construct_f32(3.5f);
  assert_true(pluto_literal_is(value, PLUTO_LITERAL_KIND_F32));
  assert_float_equal(pluto_literal_get_f32(value), 3.5f, 1e-6f);
  pluto_literal_destruct(&value);
}

void test_literal_construct_f64(void **) {
  pluto_Literal value = pluto_literal_construct_f64(3.141592653589793);
  assert_true(pluto_literal_is(value, PLUTO_LITERAL_KIND_F64));
  assert_float_equal(pluto_literal_get_f64(value), 3.141592653589793, 1e-12);
  pluto_literal_destruct(&value);
}

void test_literal_construct_string(void **) {
  pluto_Literal value = pluto_literal_construct_string(42);
  assert_true(pluto_literal_is(value, PLUTO_LITERAL_KIND_STRING));
  assert_int_equal(pluto_literal_get_string_tag(value), 42);
  pluto_literal_destruct(&value);
}

void test_literal_tuple_append_and_access(void **) {
  pluto_Literal tuple_value = pluto_literal_construct_tuple();
  pluto_LiteralTuple tuple = pluto_literal_get_tuple(tuple_value);

  assert_int_equal(pluto_literal_tuple_length(tuple), 0);

  for (uint8_t i = 0; i < 7; ++i) {
    pluto_literal_tuple_append(tuple, pluto_literal_construct_u8(i));
  }

  assert_int_equal(pluto_literal_tuple_length(tuple), 7);
  for (size_t i = 0; i < 7; ++i) {
    pluto_Literal item = pluto_literal_tuple_at(tuple, i);
    assert_true(pluto_literal_is(item, PLUTO_LITERAL_KIND_U8));
    assert_int_equal(pluto_literal_get_u8(item), i);
  }

  pluto_literal_destruct(&tuple_value);
  assert_null(tuple_value);
}

void test_literal_array_append_and_access(void **) {
  pluto_Literal array_value = pluto_literal_construct_array();
  pluto_LiteralArray array = pluto_literal_get_array(array_value);

  assert_int_equal(pluto_literal_array_length(array), 0);

  for (int32_t i = 0; i < 7; ++i) {
    pluto_literal_array_append(array, pluto_literal_construct_i32(i * 10));
  }

  assert_int_equal(pluto_literal_array_length(array), 7);
  for (size_t i = 0; i < 7; ++i) {
    pluto_Literal item = pluto_literal_array_at(array, i);
    assert_true(pluto_literal_is(item, PLUTO_LITERAL_KIND_I32));
    assert_int_equal(pluto_literal_get_i32(item), (int32_t)i * 10);
  }

  pluto_literal_destruct(&array_value);
  assert_null(array_value);
}

void test_literal_destruct_nested(void **) {
  pluto_Literal tuple_value = pluto_literal_construct_tuple();
  pluto_LiteralTuple tuple = pluto_literal_get_tuple(tuple_value);
  pluto_literal_tuple_append(tuple, pluto_literal_construct_nil());
  pluto_literal_tuple_append(tuple, pluto_literal_construct_bool(true));

  pluto_Literal nested_array_value = pluto_literal_construct_array();
  pluto_LiteralArray array = pluto_literal_get_array(nested_array_value);
  pluto_literal_array_append(array, pluto_literal_construct_u64(99));
  pluto_literal_tuple_append(tuple, nested_array_value);

  pluto_literal_destruct(&tuple_value);
  assert_null(tuple_value);
}

static char *capture_printed_literal(pluto_Literal value, pluto_Context ctx) {
  char *buffer = NULL;
  size_t buffer_len = 0;
  FILE *stream = open_memstream(&buffer, &buffer_len);
  assert_non_null(stream);
  pluto_literal_print(stream, value, ctx);
  fflush(stream);
  fclose(stream);
  return buffer;
}

typedef void (*pluto_DeathFn)(void);

static void assert_call_aborts(pluto_DeathFn fn) {
#ifdef NDEBUG
  skip();
#else
  pid_t pid = fork();
  assert_true(pid >= 0);

  if (pid == 0) {
    fn();
    _exit(0);
  }

  int32_t status = 0;
  assert_true(waitpid(pid, &status, 0) == pid);
  assert_true(WIFSIGNALED(status));
  assert_int_equal(WTERMSIG(status), SIGABRT);
#endif
}

static void call_get_i32_on_bool(void) {
  pluto_Literal value = pluto_literal_construct_bool(true);
  (void)pluto_literal_get_i32(value);
}

static void call_tuple_at_out_of_bounds(void) {
  pluto_Literal tuple_value = pluto_literal_construct_tuple();
  pluto_LiteralTuple tuple = pluto_literal_get_tuple(tuple_value);
  (void)pluto_literal_tuple_at(tuple, 0);
}

static void call_array_at_out_of_bounds(void) {
  pluto_Literal array_value = pluto_literal_construct_array();
  pluto_LiteralArray array = pluto_literal_get_array(array_value);
  (void)pluto_literal_array_at(array, 0);
}

static void call_kind_on_null(void) {
  pluto_Literal value = NULL;
  (void)pluto_literal_kind(value);
}

void test_literal_print_scalars_and_string(void **) {
  pluto_Context ctx = pluto_context_construct();
  uint32_t hello_tag = pluto_context_intern_string(ctx, PLUTO_SV("hello"));

  pluto_Literal value_nil = pluto_literal_construct_nil();
  char *printed = capture_printed_literal(value_nil, ctx);
  assert_string_equal(printed, "nil");
  free(printed);
  pluto_literal_destruct(&value_nil);

  pluto_Literal value_bool = pluto_literal_construct_bool(true);
  printed = capture_printed_literal(value_bool, ctx);
  assert_string_equal(printed, "true");
  free(printed);
  pluto_literal_destruct(&value_bool);

  pluto_Literal value_i64 = pluto_literal_construct_i64(-77);
  printed = capture_printed_literal(value_i64, ctx);
  assert_string_equal(printed, "-77");
  free(printed);
  pluto_literal_destruct(&value_i64);

  pluto_Literal value_u64 = pluto_literal_construct_u64(88);
  printed = capture_printed_literal(value_u64, ctx);
  assert_string_equal(printed, "88");
  free(printed);
  pluto_literal_destruct(&value_u64);

  pluto_Literal value_f32 = pluto_literal_construct_f32(3.5f);
  printed = capture_printed_literal(value_f32, ctx);
  assert_string_equal(printed, "3.500000");
  free(printed);
  pluto_literal_destruct(&value_f32);

  pluto_Literal value_f64 = pluto_literal_construct_f64(1.25);
  printed = capture_printed_literal(value_f64, ctx);
  assert_string_equal(printed, "1.250000");
  free(printed);
  pluto_literal_destruct(&value_f64);

  pluto_Literal value_string = pluto_literal_construct_string(hello_tag);
  printed = capture_printed_literal(value_string, ctx);
  assert_string_equal(printed, "\"hello\"");
  free(printed);
  pluto_literal_destruct(&value_string);

  pluto_context_destruct(&ctx);
}

void test_literal_print_tuple_and_array(void **) {
  pluto_Context ctx = pluto_context_construct();
  uint32_t tag = pluto_context_intern_string(ctx, PLUTO_SV("x"));

  pluto_Literal tuple_value = pluto_literal_construct_tuple();
  pluto_LiteralTuple tuple = pluto_literal_get_tuple(tuple_value);
  pluto_literal_tuple_append(tuple, pluto_literal_construct_i32(1));
  pluto_literal_tuple_append(tuple, pluto_literal_construct_string(tag));

  pluto_Literal array_value = pluto_literal_construct_array();
  pluto_LiteralArray array = pluto_literal_get_array(array_value);
  pluto_literal_array_append(array, pluto_literal_construct_bool(false));
  pluto_literal_array_append(array, tuple_value);

  char *printed = capture_printed_literal(array_value, ctx);
  assert_string_equal(printed, "[false, (1, \"x\")]");
  free(printed);

  pluto_literal_destruct(&array_value);
  pluto_context_destruct(&ctx);
}

void test_literal_assert_getter_kind_mismatch(void **) {
  assert_call_aborts(call_get_i32_on_bool);
}

void test_literal_assert_tuple_at_out_of_bounds(void **) {
  assert_call_aborts(call_tuple_at_out_of_bounds);
}

void test_literal_assert_array_at_out_of_bounds(void **) {
  assert_call_aborts(call_array_at_out_of_bounds);
}

void test_literal_assert_kind_on_null(void **) {
  assert_call_aborts(call_kind_on_null);
}

int32_t main(int32_t argc, char const *argv[]) {
  (void)argc;
  (void)argv;

  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_literal_construct_nil),
    cmocka_unit_test(test_literal_construct_bool),
    cmocka_unit_test(test_literal_construct_i8),
    cmocka_unit_test(test_literal_construct_i16),
    cmocka_unit_test(test_literal_construct_i32),
    cmocka_unit_test(test_literal_construct_i64),
    cmocka_unit_test(test_literal_construct_u8),
    cmocka_unit_test(test_literal_construct_u16),
    cmocka_unit_test(test_literal_construct_u32),
    cmocka_unit_test(test_literal_construct_u64),
    cmocka_unit_test(test_literal_construct_f32),
    cmocka_unit_test(test_literal_construct_f64),
    cmocka_unit_test(test_literal_construct_string),
    cmocka_unit_test(test_literal_tuple_append_and_access),
    cmocka_unit_test(test_literal_array_append_and_access),
    cmocka_unit_test(test_literal_destruct_nested),
    cmocka_unit_test(test_literal_print_scalars_and_string),
    cmocka_unit_test(test_literal_print_tuple_and_array),
    cmocka_unit_test(test_literal_assert_getter_kind_mismatch),
    cmocka_unit_test(test_literal_assert_tuple_at_out_of_bounds),
    cmocka_unit_test(test_literal_assert_array_at_out_of_bounds),
    cmocka_unit_test(test_literal_assert_kind_on_null),
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}