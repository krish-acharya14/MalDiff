#pragma once

#include <nlohmann/json.hpp>
#include <filesystem>
#include <string>

namespace mede::config {
struct AppConfig {
    std::string configVersion{"1.0"};

    struct Project {
        std::string name{"malware-evolution-diff-engine"};
        std::string rootDirectory{"."};
    } project;

    struct Directories {
        std::string samplesRaw{"samples/raw"};
        std::string samplesProcessed{"samples/processed"};
        std::string samplesQuarantine{"samples/quarantine"};
        std::string metadata{"metadata"};
        std::string reports{"reports"};
        std::string configs{"configs"};
        std::string logs{"logs"};
        std::string cache{"cache"};
        std::string tests{"tests"};
    } directories;

    struct Logging {
        std::string level{"info"};
        std::string logFileName{"mede.log"};
        std::size_t maxFileSizeMb{5};
        std::size_t maxFiles{3};
    } logging;

    struct Storage {
        bool createMissingDirectories{true};
        bool verifyPermissionsOnInit{true};
    } storage;

    nlohmann::json extra = nlohmann::json::object();
};

void to_json(nlohmann::json& j, const AppConfig& c);
void from_json(const nlohmann::json& j, AppConfig& c);

class ConfigManager {
  public:
    explicit ConfigManager(std::filesystem::path configPath = "configs/config.json");

    AppConfig load();

    void save(const AppConfig& config) const;

    [[nodiscard]] static AppConfig makeDefault();

    [[nodiscard]] const AppConfig& current() const { return config_; }
    [[nodiscard]] const std::filesystem::path& path() const { return configPath_; }

  private:
    std::filesystem::path configPath_;
    AppConfig config_;
};

}
