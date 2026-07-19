#pragma once 

#include <filesystem>
#include <memory>
#include "config/ConfigManager.hpp"
#include "storage/StorageManager.hpp"

namespace mede::core {
    class Application {
        public:
            explicit Application(std::filesystem::path rootDirectory = ".");

            void intializeProject();

            [[nodiscard]] const config::AppConfig& config() const {return configManager_.current();}
            [[nodiscard]] config::ConfigManager& configManager() {return configManager_;}
            [[nodiscard]] const std::filesystem::path& root() const {return rootDirectory_;}
        
        private:
            std::filesystem::path rootDirectory_;
            config::ConfigManager configManager_;
            std::unique_ptr<storage::StorageManager> storageManager_;
    };
}
