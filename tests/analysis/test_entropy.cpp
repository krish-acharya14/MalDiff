#include <gtest/gtest.h>
#include "analysis/EntropyEngine.hpp"
#include <vector>

TEST(EntropyEngineTest, EmptyData) {
    std::vector<std::byte> data;
    EXPECT_DOUBLE_EQ(mede::analysis::EntropyEngine::shannonEntropy(data), 0.0);
}

TEST(EntropyEngineTest, UniformData) {
    std::vector<std::byte> data(100, std::byte{0x41}); // All 'A's
    EXPECT_DOUBLE_EQ(mede::analysis::EntropyEngine::shannonEntropy(data), 0.0);
}

TEST(EntropyEngineTest, RandomData) {
    std::vector<std::byte> data;
    for (int i = 0; i < 256; ++i) {
        data.push_back(std::byte{static_cast<unsigned char>(i)});
    }
    EXPECT_DOUBLE_EQ(mede::analysis::EntropyEngine::shannonEntropy(data), 8.0);
}
