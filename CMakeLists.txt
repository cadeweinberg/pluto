# SPDX-License-Identifier: GPL-3.0-or-later

cmake_minimum_required(VERSION 3.10)
project(pluto C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Add include directory
set(PLUTO_BASE_DIR    "${PROJECT_SOURCE_DIR}")
set(PLUTO_SOURCE_DIR  "${PLUTO_BASE_DIR}/src")
set(PLUTO_INCLUDE_DIR "${PLUTO_BASE_DIR}/include")
set(PLUTO_TEST_DIR    "${PLUTO_BASE_DIR}/tests")
set(PLUTO_CMAKE_DIR   "${PLUTO_BASE_DIR}/cmake")

include(${PLUTO_CMAKE_DIR}/deps.cmake)

# Add subdirectories
add_subdirectory(src)

include(CTest)
add_subdirectory(tests)

