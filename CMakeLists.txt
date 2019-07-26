cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(chapro-openMHA-plugin)

set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY true)
add_subdirectory(googletest EXCLUDE_FROM_ALL)
add_subdirectory(GSL EXCLUDE_FROM_ALL)
add_library(openMHA
    openMHA/mha/libmha/src/mha_parser.cpp
    openMHA/mha/libmha/src/mha_events.cpp
    openMHA/mha/libmha/src/mha_error.cpp
    openMHA/mha/libmha/src/mha_os.cpp
    openMHA/mha/libmha/src/mha_errno.c
)
set_property(TARGET openMHA PROPERTY POSITION_INDEPENDENT_CODE ON)
target_include_directories(openMHA PUBLIC openMHA/mha/libmha/src)
target_compile_features(openMHA PRIVATE cxx_std_11)
add_library(chapro
    chapro/agc_process.c
    chapro/agc_prepare.c
    chapro/firfb_prepare.c
    chapro/firfb_process.c
    chapro/cha_core.c
    chapro/rfft.c
)
set_property(TARGET chapro PROPERTY POSITION_INDEPENDENT_CODE ON)
target_include_directories(chapro PUBLIC chapro)
add_subdirectory(chapro-openmha-plugin)
