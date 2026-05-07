# SPDX-License-Identifier: GPL-3.0-or-later

cmake_minimum_required(VERSION 3.10)

add_library(pluto-support STATIC
  cli_parser.c
  string_constant.c
  string_view.c
)

target_include_directories(pluto-support PUBLIC ${PLUTO_INCLUDE_DIR})
