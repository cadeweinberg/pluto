# SPDX-License-Identifier: GPL-3.0-or-later

cmake_minimum_required(VERSION 3.10)

add_subdirectory(support)
add_subdirectory(tiir)
add_subdirectory(context)

# Create executable 'charon'
add_executable(charon
  main.c
)

target_include_directories(charon PRIVATE ${PLUTO_INCLUDE_DIR})
target_link_libraries(charon PRIVATE pluto-support pluto-context pluto-tiir)
