#include "cli/CliHandler.hpp"

#include <iostream>
#include <spdlog/spdlog.h>
#include "config/ConfigManager.hpp"
#include "models/BinaryType.hpp"
#include "pipeline/SampleManager.hpp"
#include "common/ErrorHandling.hpp"
#include "logging/Logger.hpp"

namespace mede::cli {
    namespace {
        constexpr std::string_view kProjectVersion = "0.1.0";
    }

    CliHandler::CliHandler() {
        app_.set_version_flag("--app-version", std::string(kProjectVersion));
        app_.require_subcommand(0, 1);
        app_.add_option("--root", rootDirectory_, "Project root directory") -> default_val(".") -> option_text("PATH");

        registerInitCommand();
        registerImportCommand();
        registerPlaceholderCommand("analyze", "Analyze imported samples.");
        registerPlaceholderCommand("diff", "Difference between two sample versions.");
        registerPlaceholderCommand("timeline", "Show the evolution timeline for a sample family.");
        registerPlaceholderCommand("report", "Generate an analysis report.");
        registerVersionCommand();
    }
    void CliHandler::registerInitCommand() {
        auto* initCmd = app_.add_subcommand("init", "Initialize the project directory structure.");
        initCmd -> callback([this]() {handleInit();});
    }
    
    void CliHandler::registerImportCommand() {
        auto* importCmd = app_.add_subcommand("import", "Import a sample binary into the engine.");
        importCmd -> add_option("file", importFilePath_, "Path to the binary sample to import") -> required() -> option_text("FILE");
        importCmd -> callback([this]() { handleImport(); });
    }

    void CliHandler::registerPlaceholderCommand(const std::string& name, const std::string& description) {
        auto* cmd = app_.add_subcommand(name, description);
        cmd -> callback([name]() {std::cout << "Not implemented yet." << std::endl;});
    }

    void CliHandler::registerVersionCommand() {
        auto* versionCmd = app_.add_subcommand("version", "Print the engine version.");
        versionCmd -> callback([]() {std::cout << "MalDiff v" << kProjectVersion << std::endl;});
    }

    void CliHandler::handleInit() {
        try {
            core::Application app(rootDirectory_);
            app.intializeProject();

            std::cout << "Project initialized successfully at '" << app.root().string() << "." << std::endl;
            std::cout << " Configuration: " << app.configManager().path().string() << std::endl;
            std::cout << " Logs:        " << (app.root() / app.config().directories.logs).string() << std::endl;
        } catch (const common::MedeException& ex) {
            std::cerr << "Initialization failed: " << ex.what() << std::endl;
            throw; 
        }
    }

    void CliHandler::handleImport() {
        try {
            config::ConfigManager cfgMgr(rootDirectory_ / "configs" / "config.json");
            const auto cfg = cfgMgr.load();

            logging::Logger::Options logOpts;
            logOpts.logDirectory = rootDirectory_ / cfg.directories.logs;
            logOpts.logFileName = cfg.logging.logFileName;
            logOpts.maxFileSizeBytes = cfg.logging.maxFileSizeMb * std::size_t{1024} * std::size_t{1024};
            logOpts.maxFiles = cfg.logging.maxFiles;
            logOpts.level = spdlog::level::from_str(cfg.logging.level);
            logging::Logger::init(logOpts, false);

            pipeline::SampleManager manager(cfg, rootDirectory_);
            const auto result = manager.importSample(std::filesystem::path{importFilePath_});

            if (!result) {
                std::cerr << "\n\u2718 Import failed: " << result.error << std::endl;
                throw common::MedeException(result.error);
            }

            const auto& sample = result.value;
            const auto& attrs = sample.metadata.attributes;

            const std::string binaryTypeStr = attrs.count("binaryType") > 0 ? attrs.at("binaryType") : std::string(models::toString(models::BinaryType::Unknown));
            const std::string sizeStr = attrs.count("fileSize") > 0 ? attrs.at("fileSize") : "?";

            std::cout << "\n\u2714 Imported successfully\n"
                  << "  Sample ID:   " << sample.id << "\n"
                  << "  SHA256:      " << sample.hashes.sha256 << "\n"
                  << "  Binary Type: " << binaryTypeStr << "\n"
                  << "  Size:        " << sizeStr << " bytes\n"
                  << std::endl;
        } catch (const common::MedeException) {
            throw;
        } catch (const std::exception& ex) {
            std::cerr << "Unexpected error during import: " << ex.what() << std::endl;
            throw common::MedeException(ex.what());
        }
    }

    int CliHandler::run(int argc, char** argv) {
        try {
            app_.parse(argc, argv);
        } catch (const CLI::ParseError& e) {
            return app_.exit(e);
        } catch (const common::MedeException&) {
            return 1;
        } catch (const std::exception& ex) {
            std::cerr << "Unexpected error: " << ex.what() << std::endl;
            return 1;
        }

        return 0;
    }
    
}
