cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(chapro-openMHA-plugin)

add_subdirectory(googletest)
add_subdirectory(GSL)
add_library(openMHA
    openMHA/mha/libmha/src/mha_parser.cpp
    openMHA/mha/libmha/src/mha_events.cpp
    openMHA/mha/libmha/src/mha_error.cpp
    openMHA/mha/libmha/src/mha_os.cpp
    openMHA/mha/libmha/src/mha_errno.c
)
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
target_include_directories(chapro PUBLIC chapro)
add_subdirectory(chapro-openmha-plugin)
