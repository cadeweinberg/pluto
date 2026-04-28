# SPDX-Identifier: GPL-3.0-or-later

add_test(NAME pluto_smoke_test COMMAND $<TARGET_FILE:pluto>)

add_executable(pluto_tests
    adt/test_flat_map.cpp
)
target_include_directories(pluto_tests PRIVATE ${PLUTO_INCLUDE_DIR})
target_link_libraries(pluto_tests PRIVATE Catch2::Catch2WithMain)

catch_discover_tests(pluto_tests)