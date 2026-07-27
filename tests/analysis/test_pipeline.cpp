#include <gtest/gtest.h>
#include "analysis/AnalysisPipeline.hpp"
#include "config/ConfigManager.hpp"
#include <fstream>

TEST(AnalysisPipelineTest, MissingFile) {
    mede::config::AppConfig cfg;
    mede::analysis::AnalysisPipeline pipeline(cfg, ".");
    auto result = pipeline.analyze("nonexistent_file.exe");
    EXPECT_FALSE(result);
}

TEST(AnalysisPipelineTest, EmptyFile) {
    mede::config::AppConfig cfg;
    std::ofstream out("empty.exe");
    out.close();

    mede::analysis::AnalysisPipeline pipeline(cfg, ".");
    auto result = pipeline.analyze("empty.exe");
    EXPECT_FALSE(result);
    EXPECT_EQ(result.error, "File is empty");
    
    std::filesystem::remove("empty.exe");
}
