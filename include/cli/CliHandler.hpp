#pragma once

#include <CLI/CLI.hpp>
#include <filesystem>
#include <memory>
#include "core/Application.hpp"

namespace mede::cli {
    class CliHandler {
        public:
            CliHandler();

            int run(int argc, char** argv);
        
        private:
            CLI::App app_{"Malware Evolution Diff Engine"};
            std::filesystem::path rootDirectory_{"."};

            void registerInitCommand();
            void registerPlaceholderCommand(const std::string& name, const std::string& description);
            void registerVersionCommand();
            void handleInit();
    };
}
