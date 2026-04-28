# SPDX-Identifier: GPL-3.0-or-later

include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG b670de4fe12ac7c5e858b7de3a14fb4bd18c760e # v3.14.0
)

FetchContent_MakeAvailable(Catch2)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

find_program(re2c NAMES re2c REQUIRED)

