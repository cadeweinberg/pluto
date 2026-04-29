# SPDX-Identifier: GPL-3.0-or-later

add_library(pluto_ti_ir
    ti-ir/layout.cpp
    ti-ir/string_interner.cpp
    ti-ir/type_interner.cpp
    ti-ir/type.cpp
)
target_include_directories(pluto_ti_ir PUBLIC ${PLUTO_INCLUDE_DIR})

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
target_link_libraries(pluto_frontend PUBLIC pluto_ti_ir)

add_executable(pluto
    ${PLUTO_SOURCE_DIR}/main.cpp
)
target_include_directories(pluto PRIVATE ${PLUTO_INCLUDE_DIR})
target_link_libraries(pluto PRIVATE pluto_ti_ir)