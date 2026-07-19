#pragma once

#include <filesystem>
#include <vector>
#include "config/ConfigManager.hpp"

namespace mede::storage {
    class StorageManager {
        public:
            explicit StorageManager(const config::AppConfig& cofig, std::filesystem::path rootDirector = ".");

            std::vector<std::filesystem::path> ensureDirectoryStructure() const;

            void verifyPermissions() const;

            [[nodiscard]] std::vector<std::filesystem::path> managedDirectories() const;

            [[nodiscard]] const std::filesystem::path& root() const {return rootDirectory_;}

        private:
            config::AppConfig config_;
            std::filesystem::path rootDirectory_;

            [[nodiscard]] std::filesystem::path resolve(const std::string& relative) const;
    };
}
