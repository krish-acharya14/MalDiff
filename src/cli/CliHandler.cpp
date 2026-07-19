#include "cli/CliHandler.hpp"

#include <iostream>
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
        registerPlaceholderCommand("import", "Import samples into the engine.");
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
