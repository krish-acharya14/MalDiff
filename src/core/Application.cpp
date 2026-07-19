#include "core/Application.hpp"
#include "logging/Logger.hpp"

namespace mede::core {
    namespace fs = std::filesystem;

    Application::Application(fs::path rootDirectory) : rootDirectory_(std::move(rootDirectory)), configManager_(rootDirectory_ / "configs" / "config.json") {}

    void Application::intializeProject() {
        const auto cfg = configManager_.load();

        storageManager_ = std::make_unique<storage::StorageManager>(cfg, rootDirectory_);
        const auto created = storageManager_ -> ensureDirectoryStructure();

        logging::Logger::Options logOptions;
        logOptions.logDirectory = rootDirectory_ / cfg.directories.logs;
        logOptions.logFileName = cfg.logging.logFileName;
        logOptions.maxFileSizeBytes = cfg.logging.maxFileSizeMb * 1024 * 1024;
        logOptions.maxFiles = cfg.logging.maxFiles;
        logOptions.level = spdlog::level::from_str(cfg.logging.level);
        logging::Logger::init(logOptions, true);

        MEDE_LOG_INFO("Project initialization started at root '{}'", rootDirectory_.string());
        for (const auto& dir : created) 
            MEDE_LOG_INFO("Created directory: {}", dir.string());
        MEDE_LOG_INFO("Configuration loaded from '{}'", configManager_.path().string());

        if (cfg.storage.verifyPermissionsOnInit) {
            storageManager_ -> verifyPermissions();
            MEDE_LOG_INFO("Verified read/write permissions on all managed directories.");
        }

        MEDE_LOG_INFO("Project initialization completed successfully.");
    }
}
