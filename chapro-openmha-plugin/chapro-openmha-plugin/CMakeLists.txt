add_library(chapro-openmha-plugin SHARED chapro-openmha-plugin.cpp)
set_target_properties(chapro-openmha-plugin PROPERTIES PREFIX "")
set_target_properties(chapro-openmha-plugin PROPERTIES OUTPUT_NAME "chapro")
target_compile_options(chapro-openmha-plugin PRIVATE -Wall -Wextra -pedantic -Werror)
target_compile_features(chapro-openmha-plugin PRIVATE cxx_std_17)
target_link_libraries(chapro-openmha-plugin hearing-aid openMHA chapro GSL)
install(TARGETS chapro-openmha-plugin DESTINATION lib)

