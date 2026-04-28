# SPDX-Identifier: GPL-3.0-or-later

add_test(NAME pluto_smoke_test COMMAND $<TARGET_FILE:pluto>)

add_executable(pluto_common_tests
    adt/test_flat_map.cpp
    adt/test_string_interner.cpp
)
target_include_directories(pluto_common_tests PRIVATE ${PLUTO_INCLUDE_DIR})
target_link_libraries(pluto_common_tests PRIVATE pluto_common Catch2::Catch2WithMain)

catch_discover_tests(pluto_common_tests)


add_executable(pluto_frontend_tests
    frontend/test_lexer.cpp
)
target_include_directories(pluto_frontend_tests PRIVATE ${PLUTO_INCLUDE_DIR})
target_link_libraries(pluto_frontend_tests PRIVATE pluto_frontend Catch2::Catch2WithMain)

catch_discover_tests(pluto_frontend_tests)