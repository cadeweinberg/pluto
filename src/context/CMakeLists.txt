# SPDX-License-Identifier: GPL-3.0-or-later

cmake_minimum_required(VERSION 3.10)

add_library(pluto-context STATIC
  context.c
  string_interner.c
  value_interner.c
)

target_include_directories(pluto-context PUBLIC ${PLUTO_INCLUDE_DIR})
target_link_libraries(pluto-context PUBLIC pluto-support pluto-tiir)
