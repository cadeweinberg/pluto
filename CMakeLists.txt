# SPDX-Identifier: GPL-3.0-or-later

cmake_minimum_required(VERSION 3.20)

project(pluto VERSION 0.1.0 LANGUAGES CXX)

set(PLUTO_CMAKE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(PLUTO_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(PLUTO_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(PLUTO_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test)

include(${PLUTO_CMAKE_DIR}/deps.cmake)

include(CTest)
include(Catch)

add_subdirectory(src)

if(BUILD_TESTING)
    add_subdirectory(test)
endif()
