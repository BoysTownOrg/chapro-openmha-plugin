add_library(hearing-aid
    src/AfcHearingAid.cpp
    src/HearingAidBuilder.cpp
)
set_property(TARGET hearing-aid PROPERTY POSITION_INDEPENDENT_CODE ON)
target_include_directories(hearing-aid 
    PUBLIC include 
    PRIVATE include/hearing-aid
)
target_compile_options(hearing-aid PRIVATE 
    ${CHAPRO_OPENMHA_PLUGIN_PRIVATE_WARNINGS} 
    ${CHAPRO_OPENMHA_PLUGIN_PRIVATE_OPTIMIZATIONS}
)
target_compile_features(hearing-aid PRIVATE cxx_std_17)
target_link_libraries(hearing-aid GSL)
