# SPDX-Identifier: GPL-3.0-or-later

add_test(NAME pluto_smoke_test COMMAND $<TARGET_FILE:pluto>)

add_executable(pluto_adt_tests
    adt/test_flat_map.cpp
)
target_include_directories(pluto_adt_tests PRIVATE ${PLUTO_INCLUDE_DIR})
target_link_libraries(pluto_adt_tests PRIVATE Catch2::Catch2WithMain)

catch_discover_tests(pluto_adt_tests)

add_executable(pluto_tiir_tests
    tiir/test_string_interner.cpp
)
target_include_directories(pluto_tiir_tests PRIVATE ${PLUTO_INCLUDE_DIR})
target_link_libraries(pluto_tiir_tests PRIVATE pluto_tiir Catch2::Catch2WithMain)

catch_discover_tests(pluto_tiir_tests)


add_executable(pluto_frontend_tests
    frontend/test_lexer.cpp
)
target_include_directories(pluto_frontend_tests PRIVATE ${PLUTO_INCLUDE_DIR})
target_link_libraries(pluto_frontend_tests PRIVATE pluto_frontend Catch2::Catch2WithMain)

catch_discover_tests(pluto_frontend_tests)
