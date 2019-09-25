add_executable(google-tests
    AfcHearingAidTests.cpp
    HearingAidBuilderTests.cpp
)
target_compile_options(google-tests PRIVATE ${CHAPRO_OPENMHA_PLUGIN_PRIVATE_WARNINGS})
target_compile_features(google-tests PRIVATE cxx_std_17)
target_link_libraries(google-tests hearing-aid gtest_main)
add_test(NAME google-tests COMMAND google-tests)
