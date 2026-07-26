#pragma once

#include <filesystem>
#include <nlohmann/json.hpp>
#include "models/FeatureVector.hpp"

namespace mede::analysis {
    class ReportGenerator {
        public: 
            ReportGenerator() = delete;

            [[nodiscard]] static nlohmann::json generate(const models::FeatureVector& features);

            static void writeToFile(const nlohmann::json& report, const std::filesystem::path& path);
    };
}
