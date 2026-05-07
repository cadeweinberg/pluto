# SPDX-License-Identifier: GPL-3.0-or-later

add_library(pluto-tiir STATIC
  operand.c
  value.c
)

target_include_directories(pluto-tiir PUBLIC ${PLUTO_INCLUDE_DIR})
target_link_libraries(pluto-tiir PUBLIC pluto-support)
