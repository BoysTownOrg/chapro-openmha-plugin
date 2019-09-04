include(ExternalProject)

set(PRIVATE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)
set(PRIVATE_INSTALL_LIB ${PRIVATE_INSTALL_PREFIX}/lib)

function(add_openmha)
    set(OPENMHA_COMPILER_PREFIX "")
    set(OPENMHA_ARCH x86_64)
    if(${CMAKE_CROSSCOMPILING})
        set(OPENMHA_COMPILER_PREFIX arm-linux-gnueabihf-)
        set(OPENMHA_ARCH armhf)
    endif()
    set(OPENMHA_SOURCE_DIR ${CMAKE_SOURCE_DIR}/openMHA)
    set(OPENMHA_LIBMHA_DIR ${OPENMHA_SOURCE_DIR}/mha/libmha)
    set(OPENMHA_RELATIVE_BUILD_DIR build)
    set(OPENMHA_LIBRARY_NAME libopenmha${CMAKE_SHARED_LIBRARY_SUFFIX})
    ExternalProject_Add(
        project_openMHA
        SOURCE_DIR ${OPENMHA_SOURCE_DIR}
        BUILD_IN_SOURCE TRUE
        CONFIGURE_COMMAND 
            ${OPENMHA_SOURCE_DIR}/configure --arch=${OPENMHA_ARCH} &&
            make BUILD_DIR=${OPENMHA_RELATIVE_BUILD_DIR} clean
        BUILD_COMMAND 
            make -C ${OPENMHA_LIBMHA_DIR}
            -f ${CMAKE_SOURCE_DIR}/Makefile-libopenmha-simplified
            DYNAMIC_LIB_EXT=${CMAKE_SHARED_LIBRARY_SUFFIX} 
            COMPILERPREFIX=${OPENMHA_COMPILER_PREFIX}
            BUILD_DIR=${OPENMHA_RELATIVE_BUILD_DIR}
        INSTALL_COMMAND 
            cp -f ${OPENMHA_LIBMHA_DIR}/${OPENMHA_RELATIVE_BUILD_DIR}/${OPENMHA_LIBRARY_NAME} ${PRIVATE_INSTALL_LIB}
    )
    add_library(openMHA SHARED IMPORTED)
    set_target_properties(openMHA PROPERTIES 
        IMPORTED_LOCATION ${PRIVATE_INSTALL_LIB}/${OPENMHA_LIBRARY_NAME}
        INTERFACE_INCLUDE_DIRECTORIES ${OPENMHA_LIBMHA_DIR}/src
    )
endfunction()

function(add_chapro)
    set(CHAPRO_MAKEFILE Makefile)
    if(${CMAKE_CROSSCOMPILING})
        set(CHAPRO_MAKEFILE makefile.arm)
    endif()
    set(CHAPRO_LIBRARY_NAME libchapro.a)
    ExternalProject_Add(
        project_chapro
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/chapro
        BUILD_IN_SOURCE TRUE
        CONFIGURE_COMMAND 
            chmod +x configure &&
            ./configure &&
            make -f ${CHAPRO_MAKEFILE} clean
        BUILD_COMMAND make -f ${CHAPRO_MAKEFILE} ${CHAPRO_LIBRARY_NAME}
        INSTALL_COMMAND 
            cp -f ${CHAPRO_LIBRARY_NAME} ${PRIVATE_INSTALL_LIB}
    )
    add_library(chapro STATIC IMPORTED)
    set_target_properties(chapro PROPERTIES 
        IMPORTED_LOCATION ${PRIVATE_INSTALL_LIB}/${CHAPRO_LIBRARY_NAME}
        INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/chapro
    )
endfunction()

function(add_openmha_plugin target source plugin)
    add_library(${target} SHARED ${source})
    add_dependencies(${target} project_chapro project_openMHA)
    set_target_properties(${target} PROPERTIES PREFIX "")
    set_target_properties(${target} PROPERTIES OUTPUT_NAME ${plugin})
    target_compile_options(${target} PRIVATE 
        -Wall -Wextra -pedantic -Werror -O3
    )
    target_compile_features(${target} PRIVATE cxx_std_17)
    target_link_libraries(${target} hearing-aid openMHA chapro GSL)
    install(TARGETS ${target} DESTINATION lib)
endfunction()

function(make_directory directory)
    file(MAKE_DIRECTORY ${directory})
endfunction()

make_directory(${PRIVATE_INSTALL_PREFIX})
make_directory(${PRIVATE_INSTALL_LIB})

add_openmha()
add_chapro()
add_openmha_plugin(chapro-openmha-plugin chapro-openmha-plugin.cpp chapro)
add_openmha_plugin(chapro-afc-openmha-plugin chapro-openmha-plugin-AFC.cpp chapro_afc)