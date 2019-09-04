add_library(hearing-aid 
    src/HearingAid.cpp
    src/AfcHearingAid.cpp
)
set_property(TARGET hearing-aid PROPERTY POSITION_INDEPENDENT_CODE ON)
target_include_directories(hearing-aid PUBLIC include)
target_include_directories(hearing-aid PRIVATE include/hearing-aid)
target_compile_options(hearing-aid PRIVATE 
    -Wall -Wextra -pedantic -Werror -O3
)
target_compile_features(hearing-aid PRIVATE cxx_std_17)
target_link_libraries(hearing-aid GSL)
