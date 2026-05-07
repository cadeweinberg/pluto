# SPDX-License-Identifier: GPL-3.0-or-later

cmake_minimum_required(VERSION 3.10)

add_library(pluto-tiir STATIC
  operand.c
  value.c
)

target_include_directories(pluto-tiir PUBLIC ${PLUTO_INCLUDE_DIR})
target_link_libraries(pluto-tiir PUBLIC pluto-support pluto-context)
