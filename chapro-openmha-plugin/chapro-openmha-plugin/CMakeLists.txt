set(PRIVATE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)
set(PRIVATE_INSTALL_LIB ${PRIVATE_INSTALL_PREFIX}/lib)
file(MAKE_DIRECTORY ${PRIVATE_INSTALL_PREFIX})
file(MAKE_DIRECTORY ${PRIVATE_INSTALL_LIB})

set(OPENMHA_COMPILER_PREFIX "")
set(OPENMHA_ARCH "x86_64")
set(CHAPRO_MAKEFILE "Makefile")

if(${CMAKE_CROSSCOMPILING})
    set(OPENMHA_COMPILER_PREFIX "arm-linux-gnueabihf-")
    set(CHAPRO_MAKEFILE "makefile.arm")
    set(OPENMHA_ARCH "armhf")
endif()

set(OPENMHA_SOURCE_DIR ${CMAKE_SOURCE_DIR}/openMHA)
set(OPENMHA_LIBMHA_DIR ${OPENMHA_SOURCE_DIR}/mha/libmha)
set(OPENMHA_RELATIVE_BUILD_DIR "build")
set(LIBOPENMHA_NAME libopenmha${CMAKE_SHARED_LIBRARY_SUFFIX})

include(ExternalProject)

ExternalProject_Add(
    project_openMHA
    SOURCE_DIR ${OPENMHA_SOURCE_DIR}
    BUILD_IN_SOURCE TRUE
    CONFIGURE_COMMAND 
        ${OPENMHA_SOURCE_DIR}/configure --arch=${OPENMHA_ARCH}
    BUILD_COMMAND 
        make -C ${OPENMHA_LIBMHA_DIR}
        -f ${CMAKE_SOURCE_DIR}/Makefile-libopenmha-simplified
        DYNAMIC_LIB_EXT=${CMAKE_SHARED_LIBRARY_SUFFIX} 
        COMPILERPREFIX=${OPENMHA_COMPILER_PREFIX}
        BUILD_DIR=${OPENMHA_RELATIVE_BUILD_DIR}
    INSTALL_COMMAND 
        cp -f ${OPENMHA_LIBMHA_DIR}/${OPENMHA_RELATIVE_BUILD_DIR}/${LIBOPENMHA_NAME} ${PRIVATE_INSTALL_LIB}
)

add_library(openMHA SHARED IMPORTED)
set_target_properties(openMHA PROPERTIES 
    IMPORTED_LOCATION ${PRIVATE_INSTALL_LIB}/${LIBOPENMHA_NAME}
    INTERFACE_INCLUDE_DIRECTORIES ${OPENMHA_LIBMHA_DIR}/src)

ExternalProject_Add(
    project_chapro
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/chapro
    BUILD_IN_SOURCE TRUE
    CONFIGURE_COMMAND chmod +x configure && ./configure
    BUILD_COMMAND make -f ${CHAPRO_MAKEFILE} libchapro.a
    INSTALL_COMMAND 
        cp -f libchapro.a ${PRIVATE_INSTALL_LIB}
)

add_library(chapro STATIC IMPORTED)
set_target_properties(chapro PROPERTIES 
    IMPORTED_LOCATION ${PRIVATE_INSTALL_LIB}/libchapro.a
    INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/chapro)

add_library(chapro-openmha-plugin SHARED chapro-openmha-plugin.cpp)
add_dependencies(chapro-openmha-plugin project_chapro)
add_dependencies(chapro-openmha-plugin project_openMHA)
set_target_properties(chapro-openmha-plugin PROPERTIES PREFIX "")
set_target_properties(chapro-openmha-plugin PROPERTIES OUTPUT_NAME "chapro")
target_compile_options(chapro-openmha-plugin PRIVATE 
    -Wall -Wextra -pedantic -Werror -O3
)
target_compile_features(chapro-openmha-plugin PRIVATE cxx_std_17)
target_link_libraries(chapro-openmha-plugin hearing-aid openMHA chapro GSL)
install(TARGETS chapro-openmha-plugin DESTINATION lib)

add_library(chapro-afc-openmha-plugin SHARED chapro-openmha-plugin-AFC.cpp)
add_dependencies(chapro-afc-openmha-plugin project_chapro)
add_dependencies(chapro-afc-openmha-plugin project_openMHA)
set_target_properties(chapro-afc-openmha-plugin PROPERTIES PREFIX "")
set_target_properties(chapro-afc-openmha-plugin PROPERTIES OUTPUT_NAME "chapro-afc")
target_compile_options(chapro-afc-openmha-plugin PRIVATE 
    -Wall -Wextra -pedantic -Werror -O3
)
target_compile_features(chapro-afc-openmha-plugin PRIVATE cxx_std_17)
target_link_libraries(chapro-afc-openmha-plugin hearing-aid openMHA chapro GSL)
install(TARGETS chapro-afc-openmha-plugin DESTINATION lib)

