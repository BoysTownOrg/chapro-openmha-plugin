cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(chapro-openMHA-plugin)
option(ENABLE_TESTS "Enable tests" OFF)
if (${ENABLE_TESTS})
    enable_testing()
endif()
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY true)

add_subdirectory(googletest EXCLUDE_FROM_ALL)
add_subdirectory(GSL EXCLUDE_FROM_ALL)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

include(ExternalProject)

ExternalProject_Add(
    project_openMHA
    GIT_REPOSITORY  https://github.com/HoerTech-gGmbH/openMHA
    BUILD_IN_SOURCE TRUE
    CONFIGURE_COMMAND ./configure
    BUILD_COMMAND make DYNAMIC_LIB_EXT=${CMAKE_SHARED_LIBRARY_SUFFIX} mha/libmha
    INSTALL_COMMAND make PREFIX=${CMAKE_BINARY_DIR} install
)

add_library(openMHA SHARED IMPORTED)
set(lib_openMHA_name ${CMAKE_SHARED_LIBRARY_PREFIX}openMHA${CMAKE_SHARED_LIBRARY_SUFFIX})
set_target_properties(openMHA PROPERTIES 
    IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/lib/${lib_openMHA_name})

ExternalProject_Get_Property(project_openMHA SOURCE_DIR)
include_directories(${SOURCE_DIR}/mha/libmha/src)

ExternalProject_Add(
    project_chapro
    GIT_REPOSITORY https://github.com/boystownorg/chapro
    BUILD_IN_SOURCE TRUE
    CONFIGURE_COMMAND chmod +x configure && ./configure
    BUILD_COMMAND make libchapro.a
    INSTALL_COMMAND 
        mkdir -p ${CMAKE_BINARY_DIR}/include &&
        mkdir -p ${CMAKE_BINARY_DIR}/lib && 
        make 
        INCDIR=${CMAKE_BINARY_DIR}/include
        LIBDIR=${CMAKE_BINARY_DIR}/lib install
)

add_library(chapro STATIC IMPORTED)
set(lib_chapro_name ${CMAKE_STATIC_LIBRARY_PREFIX}chapro${CMAKE_STATIC_LIBRARY_SUFFIX})
set_target_properties(chapro PROPERTIES 
    IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/lib/${lib_chapro_name})
include_directories(${CMAKE_BINARY_DIR}/include)

add_subdirectory(chapro-openmha-plugin)
