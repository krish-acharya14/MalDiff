#include "cli/CliHandler.hpp"

#include <iostream>
#include <spdlog/spdlog.h>
#include "config/ConfigManager.hpp"

#include "utils/ErrorHandling.hpp"
#include "utils/Logger.hpp"
#include "analysis/AnalysisPipeline.hpp"
#include "diff/DiffEngine.hpp"

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
        registerAnalyzeCommand();
        registerCompareCommand();
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

    void CliHandler::registerAnalyzeCommand() {
        auto* analyzeCmd = app_.add_subcommand("analyze", "Analyze a malware sample.");
        analyzeCmd -> add_option("file", importFilePath_, "Path to the binary sample to analyze") -> required() -> option_text("FILE");
        analyzeCmd -> callback([this]() { handleAnalyze(); });
    }

    void CliHandler::registerCompareCommand() {
        auto* compareCmd = app_.add_subcommand("compare", "Compare two analyzed malware samples.");
        compareCmd -> add_option("sample1", compareFilePathA_, "Path to the first binary sample") -> required() -> option_text("FILE");
        compareCmd -> add_option("sample2", compareFilePathB_, "Path to the second binary sample") -> required() -> option_text("FILE");
        compareCmd -> callback([this]() { handleCompare(); });
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

            std::cout << "\n\u2714 Import functionality merged with analyze in Phase 1.\n" << std::endl;
        } catch (const common::MedeException&) {
            throw;
        } catch (const std::exception& ex) {
            std::cerr << "Unexpected error during import: " << ex.what() << std::endl;
            throw common::MedeException(ex.what());
        }
    }

    void CliHandler::handleAnalyze() {
        try {
            config::ConfigManager cfgMgr(rootDirectory_ / "configs" / "config.json");
            const auto cfg = cfgMgr.load();

            analysis::AnalysisPipeline pipeline(cfg, rootDirectory_);
            const auto result = pipeline.analyze(std::filesystem::path{importFilePath_});

            if (!result) {
                std::cerr << "\n\u2718 Analysis failed: " << result.error << std::endl;
                throw common::MedeException(result.error);
            }

            const auto& outcome = result.value;
            std::cout << "\n\u2714 Analysis completed successfully\n"
                      << "  Report saved to: " << outcome.reportPath.string() << "\n"
                      << "  Binary Format:   " << outcome.features.binaryFormat << "\n"
                      << "  Total Imports:   " << outcome.features.totalImportCount << "\n"
                      << "  Overall Entropy: " << outcome.features.overallEntropy << "\n"
                      << std::endl;
        } catch (const std::exception& ex) {
            std::cerr << "Unexpected error during analysis: " << ex.what() << std::endl;
            throw common::MedeException(ex.what());
        }
    }

    namespace {
        void printStringDifference(const std::string& label, const models::Difference<std::string>& diff) {
            std::cout << " " << label << " added:   " << diff.added.size() << "\n";
            for (const auto& v : diff.added) std::cout << "     +" << v << "\n";
            std::cout << "  " << label << " removed: " << diff.removed.size() << "\n";
            for (const auto& v : diff.removed) std::cout << "       - " << v << "\n";
            std::cout << "  " << label << " unchanged: " << diff.unchanged.size() << "\n";
        }

    }

    void CliHandler::handleCompare() {
        try {
            config::ConfigManager cfgMgr(rootDirectory_ / "config" / "config.json");
            const auto cfg = cfgMgr.load();
            
            analysis::AnalysisPipeline pipeline(cfg, rootDirectory_);

            const auto resultA = pipeline.analyze(std::filesystem::path{compareFilePathA_});
            if (!resultA) {
                std::cerr << "\n\u2718 Analysis of '" << compareFilePathA_ << "' failed: " << resultA.error << std::endl;
                throw common::MedeException(resultA.error);
            }

            const auto resultB = pipeline.analyze(std::filesystem::path{compareFilePathB_});
            if (!resultB) {
                std::cerr << "\n\u2718 Analysis of '" << compareFilePathB_ << "' failed: " << resultB.error << std::endl;
                throw common::MedeException(resultB.error);
            }

            const auto comparison = diff::DiffEngine::compare(resultA.value.features, resultB.value.features);

            std::cout << "\n\u2714 Comparison completed successfully\n" << "    Sample A: " << compareFilePathA_ << "\n" << "    Sample B: " << compareFilePathB_ << "\n\n";

            std::cout << "--- Hash Differences ---\n";
            std::cout << "  MD5:    " << (comparison.hashDifference.md5Equal ? "equal" : "different") << "\n";
            std::cout << "  SHA1:    " << (comparison.hashDifference.sha1Equal ? "equal" : "different") << "\n";
            std::cout << "  SHA256:    " << (comparison.hashDifference.sha256Equal ? "equal" : "different") << "\n";
            std::cout << std::endl;

            std::cout << "--- Section Differences ---\n";
            std::cout << "  Added:      " << comparison.sectionDifference.added.size() << "\n";
            for (const auto& s : comparison.sectionDifference.added) std::cout << "     + " << s.name << "\n";
            std::cout << "  Removed:    " << comparison.sectionDifference.removed.size() << "\n";
            for (const auto& s : comparison.sectionDifference.removed) std::cout << "   - " << s.name << "\n";
            std::cout << "  Modified:   " << comparison.sectionDifference.modified.size() << "\n";
            for (const auto& s : comparison.sectionDifference.modified) std::cout << "  * " << s.before.name << "\n";
            std::cout << "  Unchanged: " << comparison.sectionDifference.unchanged.size() << "\n";
            std::cout << std::endl;

            std::cout << "--- Import Differences ---\n";
            printStringDifference("DLLs", comparison.importDifference.dlls);
            printStringDifference("APIs", comparison.importDifference.apis);
            std::cout << std::endl;

            std::cout << "--- String Differences ---\n";
            printStringDifference("ASCII", comparison.stringDifference.asciiStrings);
            printStringDifference("UTF-16", comparison.stringDifference.utf16Strings);
            std::cout << std::endl;

            std::cout << "--- Entropy Differences ---\n";
            if (comparison.entropyDifference.empty()) std::cout << "    (none)\n";
            else {
                if (comparison.entropyDifference.overallChanged) {
                    const auto& o = comparison.entropyDifference.overall;
                    std::cout << "  overall: " << o.before << " -> " << o.after << " (delta " << o.delta << ")\n";
                }
                for (const auto& s : comparison.entropyDifference.sectionChanges) {
                    std::cout << "  " << s.label << ": " << s.before << " -> " << s.after << " (delta " << s.delta << ")\n";
                }
            }
            std::cout << std::endl;
        } catch (const common::MedeException&) {
            throw;
        } catch (const std::exception& ex) {
            std::cerr << "Unexpected error during comparison: " << ex.what() << std::endl;
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
