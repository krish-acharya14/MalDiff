#include <gtest/gtest.h>
#include "analysis/ImportParser.hpp"

TEST(ImportParserTest, EmptyData) {
    std::vector<std::byte> data;
    mede::models::PEInfo info;
    info.valid = true;
    auto imports = mede::analysis::ImportParser::parse(data, info);
    EXPECT_TRUE(imports.empty());
}
