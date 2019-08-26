cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project(chapro-openMHA-plugin)
option(ENABLE_TESTS "Enable tests" OFF)
if (${ENABLE_TESTS})
    enable_testing()
endif()
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY true)

add_subdirectory(googletest EXCLUDE_FROM_ALL)
add_subdirectory(GSL EXCLUDE_FROM_ALL)

set(PRIVATE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)
set(PRIVATE_INSTALL_LIB ${PRIVATE_INSTALL_PREFIX}/lib)
set(PRIVATE_INSTALL_INCLUDE ${PRIVATE_INSTALL_PREFIX}/include)
file(MAKE_DIRECTORY ${PRIVATE_INSTALL_PREFIX})
file(MAKE_DIRECTORY ${PRIVATE_INSTALL_LIB})
file(MAKE_DIRECTORY ${PRIVATE_INSTALL_INCLUDE})

set(OPENMHA_COMPILER_PREFIX "")
set(CHAPRO_MAKEFILE "Makefile")

if(${CMAKE_CROSSCOMPILING})
    set(OPENMHA_COMPILER_PREFIX "arm-linux-gnueabihf-")
    set(CHAPRO_MAKEFILE "makefile.arm")
endif()

include(ExternalProject)

ExternalProject_Add(
    project_openMHA
    GIT_REPOSITORY  https://github.com/HoerTech-gGmbH/openMHA
    BUILD_IN_SOURCE TRUE
    CONFIGURE_COMMAND ./configure
    BUILD_COMMAND make 
        DYNAMIC_LIB_EXT=${CMAKE_SHARED_LIBRARY_SUFFIX} 
        COMPILERPREFIX=${OPENMHA_COMPILER_PREFIX} mha/libmha
    INSTALL_COMMAND find ./mha/ -type f | grep -i ${CMAKE_SHARED_LIBRARY_SUFFIX}$ | xargs -i cp {} ${PRIVATE_INSTALL_LIB}
)

add_library(openMHA SHARED IMPORTED)
set_target_properties(openMHA PROPERTIES 
    IMPORTED_LOCATION ${PRIVATE_INSTALL_PREFIX}/lib/libopenMHA${CMAKE_SHARED_LIBRARY_SUFFIX})

ExternalProject_Get_Property(project_openMHA SOURCE_DIR)
include_directories(${SOURCE_DIR}/mha/libmha/src)

ExternalProject_Add(
    project_chapro
    GIT_REPOSITORY https://github.com/boystownorg/chapro
    BUILD_IN_SOURCE TRUE
    CONFIGURE_COMMAND chmod +x configure && ./configure
    BUILD_COMMAND make -f ${CHAPRO_MAKEFILE} libchapro.a
    INSTALL_COMMAND 
        cp -f libchapro.a ${PRIVATE_INSTALL_LIB} &&
        cp -f chapro.h ${PRIVATE_INSTALL_INCLUDE}
)

add_library(chapro STATIC IMPORTED)
set_target_properties(chapro PROPERTIES 
    IMPORTED_LOCATION ${PRIVATE_INSTALL_LIB}/libchapro.a)
target_include_directories(chapro INTERFACE ${PRIVATE_INSTALL_INCLUDE})

add_subdirectory(chapro-openmha-plugin)
