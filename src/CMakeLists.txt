# SPDX-Identifier: GPL-3.0-or-later

add_library(pluto_common
    adt/string_interner.cpp
)
target_include_directories(pluto_common PUBLIC ${PLUTO_INCLUDE_DIR})
set_target_properties(pluto_common PROPERTIES
    OUTPUT_NAME pluto
)

add_executable(pluto
    ${PLUTO_SOURCE_DIR}/main.cpp
)
target_include_directories(pluto PRIVATE ${PLUTO_INCLUDE_DIR})
target_link_libraries(pluto PRIVATE pluto_common)