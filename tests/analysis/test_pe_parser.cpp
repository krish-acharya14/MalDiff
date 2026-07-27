#include <gtest/gtest.h>
#include "analysis/PEParser.hpp"

TEST(PEParserTest, InvalidHeader) {
    std::vector<std::byte> data(1024, std::byte{0});
    auto info = mede::analysis::PEParser::parse(data);
    EXPECT_FALSE(info.valid);
    EXPECT_EQ(info.parseError, "Invalid DOS header magic");
}

TEST(PEParserTest, ValidDOSInvalidNT) {
    std::vector<std::byte> data(1024, std::byte{0});
    data[0] = std::byte{'M'};
    data[1] = std::byte{'Z'};
    // e_lfanew
    data[0x3C] = std::byte{0x40};
    auto info = mede::analysis::PEParser::parse(data);
    EXPECT_FALSE(info.valid);
    EXPECT_EQ(info.parseError, "Invalid NT headers magic");
}
