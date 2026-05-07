// SPDX-License-Identifier: GPL-3.0-or-later

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>

#include <cmocka.h>

#include "tiir/operand.h"

void test_operand_constant(void **) {
    pluto_Operand operand = pluto_operand_constant(42);
    assert_int_equal(operand.kind, PLUTO_OPERAND_KIND_CONSTANT);
    assert_int_equal(operand.data, 42);
}

void test_operand_register(void **) {
    pluto_Operand operand = pluto_operand_register(7);
    assert_int_equal(operand.kind, PLUTO_OPERAND_KIND_REGISTER);
    assert_int_equal(operand.data, 7);
}

void test_operand_immediate(void **) {
    pluto_Operand operand = pluto_operand_immediate(123);
    assert_int_equal(operand.kind, PLUTO_OPERAND_KIND_IMMEDIATE);
    assert_int_equal(operand.data, 123);
}

void test_operand_label(void **) {
    pluto_Operand operand = pluto_operand_label(99);
    assert_int_equal(operand.kind, PLUTO_OPERAND_KIND_LABEL);
    assert_int_equal(operand.data, 99);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_operand_constant),
        cmocka_unit_test(test_operand_register),
        cmocka_unit_test(test_operand_immediate),
        cmocka_unit_test(test_operand_label),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}