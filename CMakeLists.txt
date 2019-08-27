cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(chapro-openMHA-plugin)
option(ENABLE_TESTS "Enable tests" OFF)
if (${ENABLE_TESTS})
    enable_testing()
endif()
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY true)

add_subdirectory(googletest EXCLUDE_FROM_ALL)
add_subdirectory(GSL EXCLUDE_FROM_ALL)

add_subdirectory(chapro-openmha-plugin)
