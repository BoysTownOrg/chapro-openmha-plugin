set(
    SOURCES
    "src/HearingAid.cpp"
)

add_library(hearing-aid ${SOURCES})
target_include_directories(hearing-aid PUBLIC include)
target_include_directories(hearing-aid PRIVATE include/hearing-aid)
target_compile_options(hearing-aid PRIVATE -Wall -Wextra -pedantic -Werror)
target_link_libraries(hearing-aid GSL)
