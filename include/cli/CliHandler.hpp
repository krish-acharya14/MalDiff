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
            CLI::App app_{"MalDiff"};
            std::filesystem::path rootDirectory_{"."};
            std::string importFilePath_;
            std::string compareFilePathA_;
            std::string compareFilePathB_;

            void registerInitCommand();
            void registerImportCommand();
            void registerAnalyzeCommand();
            void registerCompareCommand();
            void registerPlaceholderCommand(const std::string& name, const std::string& description);
            void registerVersionCommand();
            void handleInit();
            void handleImport();
            void handleAnalyze();
            void handleCompare();
    };
}
