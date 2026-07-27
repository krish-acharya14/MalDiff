#include "analysis/AnalysisPipeline.hpp"
#include "analysis/PEParser.hpp"
#include "analysis/ImportParser.hpp"
#include "analysis/StringExtractor.hpp"
#include "analysis/EntropyEngine.hpp"
#include "analysis/ReportGenerator.hpp"
#include "utils/Logger.hpp"

#include <fstream>

namespace mede::analysis {
    AnalysisPipeline::AnalysisPipeline(config::AppConfig config, std::filesystem::path rootDirectory)
        : config_(std::move(config)), rootDirectory_(std::move(rootDirectory)) {}

    common::Result<AnalysisPipeline::AnalysisOutcome> AnalysisPipeline::analyze(const std::filesystem::path& sourcePath) {
        if (!std::filesystem::exists(sourcePath) || !std::filesystem::is_regular_file(sourcePath)) {
            return common::Result<AnalysisOutcome>::failure("File does not exist or is not a regular file");
        }

        std::ifstream file(sourcePath, std::ios::binary | std::ios::ate);
        if (!file) {
            return common::Result<AnalysisOutcome>::failure("Failed to open file for reading");
        }

        auto fileSize = file.tellg();
        if (fileSize == 0) {
            return common::Result<AnalysisOutcome>::failure("File is empty");
        }

        file.seekg(0, std::ios::beg);
        std::vector<std::byte> buffer(static_cast<std::size_t>(fileSize));
        if (!file.read(reinterpret_cast<char*>(buffer.data()), fileSize)) {
            return common::Result<AnalysisOutcome>::failure("Failed to read file contents");
        }

        models::FeatureVector features;

        // Metadata
        features.metadata.originalFileName = sourcePath.filename().string();
        features.metadata.sourcePath = sourcePath.string();
        features.metadata.attributes["fileSize"] = std::to_string(fileSize);

        // For phase 1, basic hashing can be done manually or stubbed since we don't have HashEngine anymore
        // Actually we can just stub it since the engine was deleted or we can keep a simple one.
        // Wait, did I delete HashEngine? I deleted `pipeline` where HashEngine was.
        features.hashes.sha256 = "N/A - Phase 1"; // Simplification for now

        // Type Detection & PE Parsing
        std::span<const std::byte> dataSpan{buffer};
        features.pe = PEParser::parse(dataSpan);
        if (features.pe.valid) {
            features.binaryFormat = "PE";
            features.imports = ImportParser::parse(dataSpan, features.pe);
            for (const auto& imp : features.imports) {
                features.totalImportCount += imp.functions.size();
            }
        } else {
            features.binaryFormat = "Unknown";
            // Do not fail entirely, gracefully handle unsupported/malformed.
        }

        // Strings
        features.strings = StringExtractor::extractAll(dataSpan, 5); // min length 5

        // Entropy
        features.overallEntropy = EntropyEngine::shannonEntropy(dataSpan);

        // Report
        auto reportJson = ReportGenerator::generate(features);
        auto reportPath = rootDirectory_ / config_.directories.reports / (features.metadata.originalFileName + ".json");
        ReportGenerator::writeToFile(reportJson, reportPath);

        AnalysisOutcome outcome;
        outcome.features = features;
        outcome.reportPath = reportPath;

        return common::Result<AnalysisOutcome>::success(outcome);
    }
}
