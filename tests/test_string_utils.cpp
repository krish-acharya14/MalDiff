#include <gtest/gtest.h>

#include "utils/StringUtils.hpp"

using mede::utils::StringUtils;

namespace {

TEST(StringUtilsTest, ToLowerAndUpper) {
    EXPECT_EQ(StringUtils::tolower("HeLLo"), "hello");
    EXPECT_EQ(StringUtils::toUpper("HeLLo"), "HELLO");
}

TEST(StringUtilsTest, Trim) {
    EXPECT_EQ(StringUtils::trim("  hello  "), "hello");
    EXPECT_EQ(StringUtils::trim("\t\nhello\r\n"), "hello");
    EXPECT_EQ(StringUtils::trim(""), "");
}

TEST(StringUtilsTest, Split) {
    const auto tokens = StringUtils::split("a,b,,c", ',');
    ASSERT_EQ(tokens.size(), 4u);
    EXPECT_EQ(tokens[0], "a");
    EXPECT_EQ(tokens[1], "b");
    EXPECT_EQ(tokens[2], "");
    EXPECT_EQ(tokens[3], "c");
}

TEST(StringUtilsTest, StartsAndEndsWith) {
    EXPECT_TRUE(StringUtils::startsWith("hello_world", "hello"));
    EXPECT_FALSE(StringUtils::startsWith("hello_world", "world"));
    EXPECT_TRUE(StringUtils::endsWith("hello_world", "world"));
    EXPECT_FALSE(StringUtils::endsWith("hello_world", "hello"));
}

}  
