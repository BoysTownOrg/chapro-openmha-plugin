cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(chapro-openMHA-plugin)

add_subdirectory(googletest)
add_subdirectory(GSL)
add_library(openMHA INTERFACE)
target_include_directories(openMHA INTERFACE openMHA/mha/libmha/src)
add_subdirectory(chapro-openmha-plugin)
