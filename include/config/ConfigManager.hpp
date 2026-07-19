#pragma once

#include <nlohmann/json.hpp>
#include <filesystem>
#include <string>

namespace mede::config {
    struct AppConfig {
        std::string configVersion{"1.0"};

        struct Project {
            std::string name {"malware-evolution-diff-engine"};
            std::string rootDirectory{"."};
        } project;

        struct Directories {
            std::string samplesRaw{"samples/raw"};
            std::string samplesProcessed{"samples/processed"};
        }
    }
}
