set(
    SOURCES
    "chapro-openmha-plugin.cpp"
)

add_library(chapro-openmha-plugin SHARED ${SOURCES})
target_compile_options(chapro-openmha-plugin PRIVATE -Wall -Wextra -pedantic -Werror)
target_link_libraries(chapro-openmha-plugin hearing-aid openMHA chapro)

