set(
    TESTS
    "HearingAidTests.cpp"
)
add_executable(google-tests ${TESTS})
target_compile_options(google-tests PRIVATE -Wall -Wextra -pedantic -Werror)
target_compile_features(google-tests PRIVATE cxx_std_17)
target_link_libraries(google-tests hearing-aid gtest_main GSL)
