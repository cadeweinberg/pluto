# SPDX-License-Identifier: GPL-3.0-or-later

# Add subdirectories for libraries
# Order matters for circular dependencies
add_subdirectory(support)
add_subdirectory(tiir)
add_subdirectory(context)

# Add context link dependency to tiir after context is defined
target_link_libraries(pluto-tiir PUBLIC pluto-context)

# Create executable 'charon'
add_executable(charon
  main.c
)

target_include_directories(charon PRIVATE ${PLUTO_INCLUDE_DIR})
target_link_libraries(charon PRIVATE pluto-support pluto-context pluto-tiir)
