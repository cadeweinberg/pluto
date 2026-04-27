# SPDX-Identifier: GPL-3.0-or-later

cmake_minimum_required(VERSION 3.20)

project(pluto VERSION 0.1.0 LANGUAGES CXX)

include(CTest)

add_subdirectory(src)

if(BUILD_TESTING)
    add_subdirectory(test)
endif()