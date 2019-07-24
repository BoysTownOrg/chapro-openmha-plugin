cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
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
target_compile_features(openMHA PRIVATE cxx_std_17)
add_subdirectory(chapro-openmha-plugin)
