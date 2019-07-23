set(
    SOURCES
    "src/HearingAid.cpp"
)

add_library(chapro-openmha-plugin ${SOURCES})
target_include_directories(chapro-openmha-plugin PUBLIC include)
target_include_directories(chapro-openmha-plugin PRIVATE include/chapro-openmha-plugin)
target_compile_options(chapro-openmha-plugin PRIVATE -Wall -Wextra -pedantic -Werror)
target_link_libraries(chapro-openmha-plugin GSL)
