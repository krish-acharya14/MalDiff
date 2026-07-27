#include <gtest/gtest.h>
#include "analysis/StringExtractor.hpp"
#include <vector>
#include <string>

TEST(StringExtractorTest, ExtractAscii) {
    std::string text("Hello\0World12345\0", 17);
    std::vector<std::byte> data(text.size());
    for(size_t i=0; i<text.size(); ++i) data[i] = std::byte{static_cast<unsigned char>(text[i])};

    auto results = mede::analysis::StringExtractor::extractAscii(data, 5);
    ASSERT_EQ(results.size(), 2);
    EXPECT_EQ(results[0].value, "Hello");
    EXPECT_EQ(results[1].value, "World12345");
}

TEST(StringExtractorTest, ExtractUtf16Le) {
    std::vector<std::byte> data = {
        std::byte{'H'}, std::byte{0}, std::byte{'i'}, std::byte{0}, std::byte{'!'}, std::byte{0}, std::byte{0}, std::byte{0},
        std::byte{'a'}, std::byte{0}, std::byte{'b'}, std::byte{0}, std::byte{'c'}, std::byte{0}, std::byte{'d'}, std::byte{0}
    };

    auto results = mede::analysis::StringExtractor::extractUtf16Le(data, 3);
    ASSERT_EQ(results.size(), 2);
    EXPECT_EQ(results[0].value, "Hi!");
    EXPECT_EQ(results[1].value, "abcd");
}
