# SPDX-Identifier: GPL-3.0-or-later

add_library(pluto_common
    adt/string_interner.cpp
)
target_include_directories(pluto_common PUBLIC ${PLUTO_INCLUDE_DIR})

add_custom_command(
    OUTPUT ${PLUTO_SOURCE_DIR}/frontend/lexer.cpp
    COMMAND ${re2c}
        ${PLUTO_SOURCE_DIR}/frontend/lexer.re2c 
        --output ${PLUTO_SOURCE_DIR}/frontend/lexer.cpp
        --input-encoding utf8
        --utf8
        -W
    VERBATIM
    DEPENDS ${PLUTO_SOURCE_DIR}/frontend/lexer.re2c
    COMMENT "Generating Lexer.cpp"
)

add_library(pluto_frontend
    frontend/lexer.cpp
)
target_include_directories(pluto_frontend PUBLIC ${PLUTO_INCLUDE_DIR})
target_link_libraries(pluto_frontend PUBLIC pluto_common)

add_executable(pluto
    ${PLUTO_SOURCE_DIR}/main.cpp
)
target_include_directories(pluto PRIVATE ${PLUTO_INCLUDE_DIR})
target_link_libraries(pluto PRIVATE pluto_common)