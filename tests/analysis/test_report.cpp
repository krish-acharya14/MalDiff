#include <gtest/gtest.h>
#include "analysis/ReportGenerator.hpp"
#include "models/Report.hpp"

TEST(ReportGeneratorTest, GenerateJson) {
    mede::models::FeatureVector features;
    features.metadata.originalFileName = "test.exe";
    features.overallEntropy = 4.5;
    
    auto json = mede::analysis::ReportGenerator::generate(features);
    EXPECT_EQ(json["features"]["metadata"]["originalFileName"], "test.exe");
    EXPECT_DOUBLE_EQ(json["features"]["overallEntropy"].get<double>(), 4.5);
    EXPECT_TRUE(json.contains("version"));
    EXPECT_TRUE(json.contains("generatedAtIso8601"));
}
