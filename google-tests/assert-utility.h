#ifndef ASSERT_UTILITY_H_
#define ASSERT_UTILITY_H_

#include <gtest/gtest.h>
#include <string>
#include <vector>

template<typename T>
void assertEqual(const T &expected, const T &actual) {
    EXPECT_EQ(expected, actual);
}

template<typename T>
void assertEqual(
    const std::vector<T> &expected,
    const std::vector<T> &actual
) {
    assertEqual(expected.size(), actual.size());
    using size_type = typename std::vector<T>::size_type;
    for (size_type i{ 0 }; i < expected.size(); ++i)
        assertEqual(expected.at(i), actual.at(i));
}

inline void assertEqual(
    const std::string &expected,
    const std::string &actual
) {
    EXPECT_EQ(expected, actual);
}

inline void assertTrue(bool c) {
    EXPECT_TRUE(c);
}

inline void assertFalse(bool c) {
    EXPECT_FALSE(c);
}

#endif
