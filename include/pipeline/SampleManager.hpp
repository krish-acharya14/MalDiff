#pragma once;

#include <filesystem>
#include <string>
#include "common/ErrorHandling.hpp"
#include "config/ConfigManager.hpp"
#include "models/Sample.hpp"

namespace mede::pipeline {
    class SampleManager {
        public:
            explicit SampleManager(const config::AppConfig& config, std::filesystem::path rootDirector = ".");

            [[nodiscard]] common::Result<models::Sample> importSample(const std::filesystem::path& filePath);

        private:
            config::AppConfig config_;
            std::filesystem::path rootDirectory_;

            [[nodiscard]] std::filesystem::path metadataDir() const;
            
            [[nodiscard]] std::filesystem::path rawSampleDir() const;

            [[nodiscard]] common::EntityId nextId() const;

            [[nodiscard]] bool isDuplicate(const std::string& sha256) const;

            void saveSampleMetadata(const models::Sample& sample) const;
    };

}
