add_library(chapro-openmha-plugin SHARED chapro-openmha-plugin.cpp)
target_compile_options(chapro-openmha-plugin PRIVATE -Wall -Wextra -pedantic -Werror)
target_compile_features(chapro-openmha-plugin PRIVATE cxx_std_17)
target_link_libraries(chapro-openmha-plugin hearing-aid openMHA chapro)

