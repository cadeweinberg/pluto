# SPDX-License-Identifier: GPL-3.0-or-later

cmake_minimum_required(VERSION 3.15)
project(pluto C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Add include directory
set(PLUTO_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories(${PLUTO_INCLUDE_DIR})

include(FetchContent)

FetchContent_Declare(
  cmocka
  GIT_REPOSITORY https://gitlab.com/cmocka/cmocka.git
  GIT_TAG fefa2b8a023121f7235e18ed17249e4012dd144f # v2.0.2
)

FetchContent_MakeAvailable(cmocka)

# Add subdirectories
add_subdirectory(src)

include(CTest)
add_subdirectory(tests)

