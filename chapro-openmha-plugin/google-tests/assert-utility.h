#ifndef ASSERT_UTILITY_H_
#define ASSERT_UTILITY_H_

#include <gtest/gtest.h>
#include <string>

template<typename T>
void assertEqual(const T &expected, const T &actual) {
    EXPECT_EQ(expected, actual);
}

template<typename T>
void assertEqual(
    const std::vector<T> &expected,
    const std::vector<T> &actual
) {
    using size_type = typename std::vector<T>::size_type;
    assertEqual<size_type>(expected.size(), actual.size());
    for (size_type i{ 0 }; i < expected.size(); ++i)
        assertEqual<T>(expected.at(i), actual.at(i));
}

inline void assertEqual(const std::string &expected, const std::string &actual) {
    EXPECT_EQ(expected, actual);
}

#endif
