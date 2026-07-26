#pragma once 

#include <filesystem>
#include "common/ErrorHandling.hpp"
#include "config/ConfigManager.hpp"
#include "models/FeatureVector.hpp"

namespace mede::analysis {
    class AnalysisPipeline {
        public:
            struct AnalysisOutcome {
                models::FeatureVector features;
                std::filesystem::path reportPath;
            };

            AnalysisPipeline(config::AppConfig config, std::filesystem::path rootDirectory);

            [[nodiscard]] common::Result<AnalysisOutcome> analyze(const std::filesystem::path& sourcePath);

        private:
            config::AppConfig config_;
            std::filesystem::path rootDirectory_;
    };
}
