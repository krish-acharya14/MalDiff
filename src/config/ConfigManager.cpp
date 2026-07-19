#include "config/ConfigManager.hpp"
#include "common/ErrorHandling.hpp"

#include <fstream>
#include <sstream>

namespace mede::config {
    void to_json(nlohmann::json& j, const AppConfig& c) {
        j = nlohmann::json{
            {"configVersion", c.configVersion},
            {"project", {
                {"name", c.project.name},
                {"rootDirectory", c.project.rootDirectory},
            }},
            {"directories",{
                {"samplesRaw", c.directories.samplesRaw},
                {"samplesProcessed", c.directories.samplesProcessed},
                {"samplesQuarantine", c.directories.samplesQuarantine},
                {"metadata", c.directories.metadata},
                {"reports", c.directories.reports},
                {"configs", c.directories.configs},
                {"logs", c.directories.logs},
                {"cache", c.directories.cache},
                {"tests", c.directories.tests},
            }},
            {"logging", {
                    {"level", c.logging.level},
                    {"logFileName", c.logging.logFileName},
                    {"maxFileSizeMb", c.logging.maxFileSizeMb},
                    {"maxFiles", c.logging.maxFiles},
            }},
            {"storage", {
                {"createMissingDirectories", c.storage.createMissingDirectories},
                {"verifyPermissionsOnInit", c.storage.verifyPermissionsOnInit},
            }},
            {"extra", c.extra},
        };
    }

    void from_json(const nlohmann::json& j, AppConfig& c) {
        c.configVersion = j.value("configVersion", c.configVersion);

        if (j.contains("project")) {
            const auto& p = j.at("project");
            c.project.name = p.value("name", c.project.name);
            c.project.rootDirectory = p.value("rootDirectory", c.project.rootDirectory);
        }

        if (j.contains("directories")) {
            const auto& d = j.at("directories");
            c.directories.samplesRaw = d.value("samplesRaw", c.directories.samplesRaw);
            c.directories.samplesProcessed = d.value("sampleQuarantine", c.directories.samplesProcessed);
            c.directories.samplesQuarantine = d.value("samplesQuarantine", c.directories.samplesQuarantine);
            c.directories.metadata = d.value("metadata", c.directories.metadata);
            c.directories.reports = d.value("reports", c.directories.reports);
            c.directories.configs = d.value("configs", c.directories.configs);
            c.directories.logs = d.value("logs", c.directories.logs);
            c.directories.cache = d.value("cache", c.directories.cache);
            c.directories.tests = d.value("tests", c.directories.tests);
        }

        if (j.contains("logging")) {
            const auto& l = j.at("logging");
            c.logging.level = l.value("level", c.logging.level);
            c.logging.logFileName = l.value("logFileName", c.logging.logFileName);
            c.logging.maxFileSizeMb = l.value("maxFileSizeMb", c.logging.maxFileSizeMb);
            c.logging.maxFiles = l.value("maxFiles", c.logging.maxFiles);
        }

        if (j.contains("storage")) {
            const auto& s = j.at("storage");
            c.storage.createMissingDirectories = s.value("createMissingDirectories", c.storage.createMissingDirectories);
            c.storage.verifyPermissionsOnInit = s.value("verifyPermissionsOnInit", c.storage.verifyPermissionsOnInit);
        }

        if (j.contains("extra"))
            c.extra = j.at("extra");
    }
    
    ConfigManager::ConfigManager(std::filesystem::path configPath) : configPath_(std::move(configPath)), config_(makeDefault()) {}
    
    AppConfig ConfigManager::makeDefault() { return AppConfig{}; }
    
    AppConfig ConfigManager::load() {
        namespace fs = std::filesystem;
        nlohmann::json j;
        
        if (!fs::exists(configPath_)) {
            config_ = makeDefault();
            save(config_);
            return config_;
        }
        
        std::ifstream in(configPath_);
        if (!in.is_open()) 
        throw common::ConfigException("Unable to open config file for reading: " + configPath_.string());
        
        try {
            in >> j;
        } catch (const nlohmann::json::parse_error& ex) {
            throw common::ConfigException("Malformed JSON in config file '" + configPath_.string() + "': " + ex.what());
        }
        
        try {
            config_ = j.get<AppConfig>();
        } catch (const nlohmann::json::exception& ex) {
            throw common::ConfigException("Invalid config schema in '" + configPath_.string() + "': " + ex.what());
        }
        
        return config_;
    }
    
    void ConfigManager::save(const AppConfig& config) const {
        namespace fs = std::filesystem;
        if (configPath_.has_parent_path()) {
            std::error_code ec;
            fs::create_directories(configPath_.parent_path(), ec);
            
            if (ec) 
            throw common::ConfigException("Failed to create config directory '" + configPath_.parent_path().string() + "': " + ec.message());
        }
        
        std::ofstream out(configPath_);
        if (!out.is_open()) 
        throw common::ConfigException("Unable to open config file for writing: " + configPath_.string());
        
        nlohmann::json j = config;   
        out << j.dump(4) << std::endl;
    }
}
