set(PRIVATE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)
set(PRIVATE_INSTALL_LIB ${PRIVATE_INSTALL_PREFIX}/lib)
set(PRIVATE_INSTALL_INCLUDE ${PRIVATE_INSTALL_PREFIX}/include)
file(MAKE_DIRECTORY ${PRIVATE_INSTALL_PREFIX})
file(MAKE_DIRECTORY ${PRIVATE_INSTALL_LIB})
file(MAKE_DIRECTORY ${PRIVATE_INSTALL_INCLUDE})

set(OPENMHA_COMPILER_PREFIX "")
set(OPENMHA_ARCH "x86_64")
set(CHAPRO_MAKEFILE "Makefile")

if(${CMAKE_CROSSCOMPILING})
    set(OPENMHA_COMPILER_PREFIX "arm-linux-gnueabihf-")
    set(CHAPRO_MAKEFILE "makefile.arm")
    set(OPENMHA_ARCH "armhf")
endif()

include(ExternalProject)

ExternalProject_Add(
    project_openMHA
    GIT_REPOSITORY  https://github.com/HoerTech-gGmbH/openMHA
    BUILD_IN_SOURCE TRUE
    CONFIGURE_COMMAND 
        ./configure --arch=${OPENMHA_ARCH} && 
        cp -f ${CMAKE_SOURCE_DIR}/Makefile-libopenmha-simplified mha/libmha
    BUILD_COMMAND make -C mha/libmha/ -f Makefile-libopenmha-simplified
        DYNAMIC_LIB_EXT=${CMAKE_SHARED_LIBRARY_SUFFIX} 
        COMPILERPREFIX=${OPENMHA_COMPILER_PREFIX} 
        ARCH=${OPENMHA_ARCH}
        BUILD_DIR=build
    INSTALL_COMMAND 
        cp -f mha/libmha/build/libopenMHA${CMAKE_SHARED_LIBRARY_SUFFIX} ${PRIVATE_INSTALL_LIB} &&
        find mha/libmha/src/ -type f -name "*.h*" -exec cp {} ${PRIVATE_INSTALL_INCLUDE} $<SEMICOLON>
)

add_library(openMHA SHARED IMPORTED)
set_target_properties(openMHA PROPERTIES 
    IMPORTED_LOCATION ${PRIVATE_INSTALL_LIB}/libopenMHA${CMAKE_SHARED_LIBRARY_SUFFIX})
target_include_directories(openMHA INTERFACE ${PRIVATE_INSTALL_INCLUDE})

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

add_library(chapro-openmha-plugin SHARED chapro-openmha-plugin.cpp)
add_dependencies(chapro-openmha-plugin project_chapro)
add_dependencies(chapro-openmha-plugin project_openMHA)
set_target_properties(chapro-openmha-plugin PROPERTIES PREFIX "")
set_target_properties(chapro-openmha-plugin PROPERTIES OUTPUT_NAME "chapro")
target_compile_options(chapro-openmha-plugin PRIVATE -Wall -Wextra -pedantic -Werror -O3)
target_compile_features(chapro-openmha-plugin PRIVATE cxx_std_17)
target_link_libraries(chapro-openmha-plugin hearing-aid openMHA chapro GSL)
install(TARGETS chapro-openmha-plugin DESTINATION lib)

