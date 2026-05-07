# SPDX-License-Identifier: GPL-3.0-or-later

cmake_minimum_required(VERSION 3.10)

# Test: cli-parser
add_executable(test-cli-parser
  src/support/cli_parser.c
)
target_include_directories(test-cli-parser PRIVATE ${PLUTO_INCLUDE_DIR})
target_link_libraries(test-cli-parser PRIVATE pluto-support cmocka)
add_test(NAME cli-parser COMMAND test-cli-parser)

# Test: string-view
add_executable(test-string-view
  src/support/string_view.c
)
target_include_directories(test-string-view PRIVATE ${PLUTO_INCLUDE_DIR})
target_link_libraries(test-string-view PRIVATE pluto-support cmocka)
add_test(NAME string-view COMMAND test-string-view)

# Test: string-constant
add_executable(test-string-constant
  src/support/string_constant.c
)
target_include_directories(test-string-constant PRIVATE ${PLUTO_INCLUDE_DIR})
target_link_libraries(test-string-constant PRIVATE pluto-support cmocka)
add_test(NAME string-constant COMMAND test-string-constant)

# Test: string-interner
add_executable(test-string-interner
  src/context/string_interner.c
)
target_include_directories(test-string-interner PRIVATE ${PLUTO_INCLUDE_DIR})
target_link_libraries(test-string-interner PRIVATE pluto-support pluto-context cmocka)
add_test(NAME string-interner COMMAND test-string-interner)

# Test: operand
add_executable(test-operand
  src/tiir/operand.c
)
target_include_directories(test-operand PRIVATE ${PLUTO_INCLUDE_DIR})
target_link_libraries(test-operand PRIVATE pluto-tiir cmocka)
add_test(NAME operand COMMAND test-operand)

# Test: value
add_executable(test-value
  src/tiir/value.c
)
target_include_directories(test-value PRIVATE ${PLUTO_INCLUDE_DIR})
target_link_libraries(test-value PRIVATE pluto-support pluto-context pluto-tiir cmocka)
add_test(NAME value COMMAND test-value)
