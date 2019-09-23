include(ExternalProject)

set(EXTERNAL_PROJECT_INSTALL_DIR ${CMAKE_BINARY_DIR}/external-project-install)
set(EXTERNAL_PROJECT_LIBRARY_DIR ${EXTERNAL_PROJECT_INSTALL_DIR}/lib)

function(add_external_static_target target library_name include_dir)
    add_library(${target} STATIC IMPORTED)
    set_target_properties(${target} PROPERTIES 
        IMPORTED_LOCATION ${EXTERNAL_PROJECT_LIBRARY_DIR}/${library_name}
        INTERFACE_INCLUDE_DIRECTORIES ${include_dir}
    )
endfunction()

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
    set(OPENMHA_BUILD_DIR ${OPENMHA_LIBMHA_DIR}/${OPENMHA_RELATIVE_BUILD_DIR})
    set(OPENMHA_LIBRARY ${OPENMHA_BUILD_DIR}/${OPENMHA_LIBRARY_NAME})
    ExternalProject_Add(
        project_openMHA
        SOURCE_DIR ${OPENMHA_SOURCE_DIR}
        BUILD_BYPRODUCTS ${EXTERNAL_PROJECT_LIBRARY_DIR}/${OPENMHA_LIBRARY_NAME}
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
            cp -f ${OPENMHA_LIBRARY} ${EXTERNAL_PROJECT_LIBRARY_DIR}
    )
    add_library(openMHA SHARED IMPORTED)
    set_target_properties(openMHA PROPERTIES 
        IMPORTED_LOCATION ${EXTERNAL_PROJECT_LIBRARY_DIR}/${OPENMHA_LIBRARY_NAME}
        INTERFACE_INCLUDE_DIRECTORIES ${OPENMHA_LIBMHA_DIR}/src
        IMPORTED_NO_SONAME TRUE
    )
    add_dependencies(openMHA project_openMHA)
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
        BUILD_BYPRODUCTS ${EXTERNAL_PROJECT_LIBRARY_DIR}/${CHAPRO_LIBRARY_NAME}
        BUILD_IN_SOURCE TRUE
        CONFIGURE_COMMAND
            chmod +x configure &&
            ./configure &&
            make -f ${CHAPRO_MAKEFILE} clean
        BUILD_COMMAND make -f ${CHAPRO_MAKEFILE} ${CHAPRO_LIBRARY_NAME}
        INSTALL_COMMAND
            cp -f ${CHAPRO_LIBRARY_NAME} ${EXTERNAL_PROJECT_LIBRARY_DIR}
    )
    add_external_static_target(chapro
        ${CHAPRO_LIBRARY_NAME}
        ${CMAKE_SOURCE_DIR}/chapro
    )
    add_dependencies(chapro project_chapro)
endfunction()

function(add_openmha_plugin target source plugin)
    add_library(${target} SHARED ${source})
    set_target_properties(${target} PROPERTIES 
        PREFIX ""
        OUTPUT_NAME ${plugin}
        SKIP_BUILD_RPATH TRUE
    )
    target_compile_options(${target} 
        PRIVATE -Wall -Wextra -pedantic -Werror -O3
    )
    target_compile_features(${target} PRIVATE cxx_std_17)
    target_link_libraries(${target} hearing-aid openMHA chapro GSL)
    install(TARGETS ${target} DESTINATION lib)
endfunction()

function(make_directory directory)
    file(MAKE_DIRECTORY ${directory})
endfunction()

make_directory(${EXTERNAL_PROJECT_INSTALL_DIR})
make_directory(${EXTERNAL_PROJECT_LIBRARY_DIR})

add_openmha()
add_chapro()
add_openmha_plugin(chapro-openmha-plugin 
    chapro-openmha-plugin.cpp 
    chapro
)
add_openmha_plugin(chapro-afc-openmha-plugin 
    chapro-openmha-plugin-AFC.cpp 
    chapro_afc
)
