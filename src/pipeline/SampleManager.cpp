#include "pipeline/SampleManager.hpp"
#include "logging/Logger.hpp"
#include "models/AnalysisStatus.hpp"
#include "pipeline/BinaryLoader.hpp"
#include "pipeline/FileValidator.hpp"
#include "pipeline/HashEngine.hpp"
#include "pipeline/MagicDetector.hpp"
#include "pipeline/MetadataExtractor.hpp"

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

namespace mede::pipeline {
    namespace fs = std::filesystem;

    SampleManager::SampleManager(const config::AppConfig& config, fs::path rootDirectory) : config_(config), rootDirectory_(std::move(rootDirectory)) {}

    fs::path SampleManager::metadataDir() const {
        return rootDirectory_ / config_.directories.metadata;
    }

    fs::path SampleManager::rawSampleDir() const {
        return rootDirectory_ / config_.directories.samplesRaw;
    }

    common::EntityId SampleManager::nextId() const {
        common::EntityId maxId = 0;
        std::error_code ec;

        if (!fs::exists(metadataDir(), ec) || ec) return 1;

        for (const auto& entry : fs::directory_iterator(metadataDir(), ec)) {
            if (ec) break;
            if (!entry.is_regular_file()) continue;
            if (entry.path().extension() != ".json") continue;

            const auto stem = entry.path().stem().string();
            try {
                const auto parsed = std::stoul(stem);

                if (parsed <= std::numeric_limits<common::EntityId>::max()) {
                    const auto asId = static_cast<common::EntityId>(parsed);
                    if (asId > maxId) maxId = asId;
                }
           } catch(...) {
                // Non-numeric stem
           }
        }

        return static_cast<common::EntityId>(maxId + 1u);
    }

    bool SampleManager::isDuplicate(const std::string& sha256) const {
        std::error_code ec;
        if (!fs::exists(metadataDir(), ec) || ec) return false;

        for (const auto& entry: fs::directory_iterator(metadataDir(), ec)) {
            if (ec) break;
            if (!entry.is_regular_file()) continue;
            if (entry.path().extension() != ".json") continue;

            std::ifstream in(entry.path());
            if (!in.is_open()) continue;
            
            try {
                const auto j = nlohmann::json::parse(in);
                if (!j.contains("hashes")) continue;
                if (j["hashes"].value("sha256", std::string{}) == sha256) return true;
            } catch(...) {
                // Malformed JSON
            }
        }

        return false;
    }

    void SampleManager::saveSampleMetadata(const models::Sample& sample) const {
        std::error_code ec;
        fs::create_directories(metadataDir(), ec);

        const auto metaPath = metadataDir() / (std::to_string(sample.id) + ".json");
        std::ofstream out(metaPath);

        if (!out.is_open())
            throw common::StorageException("Cannot write metadata fie: " + metaPath.string());

        const nlohmann::json j = sample;
        out << j.dump(4) << "\n";
    }

    common::Result<models::Sample> SampleManager::importSample(const fs::path& filePath) {
        // Binary Loading
        std::cout << "Loading binary..." << std::endl;
        MEDE_LOG_INFO("Import started for '{}'", filePath.string());

        auto validResult = FileValidator::validate(filePath);
        if (!validResult) {
            MEDE_LOG_ERROR("Validation failed: {}", validResult.error);
            return common::Result<models::Sample>::failure(validResult.error);
        }
        const fs::path canonicalPath = validResult.value;

        // Validaing file
        std::cout << "Validating..." << std::endl;
        auto loadResult = BinaryLoader::load(canonicalPath);
        if (!loadResult) {
            MEDE_LOG_ERROR("Load failed: {}", loadResult.error);
            return common::Result<models::Sample>::failure(loadResult.error);
        }
        auto binary = std::move(loadResult.value);

        // File format detection
        std::cout << "Detecting format..." << std::endl;
        binary.binaryType = MagicDetector::detect(binary.data);
        MEDE_LOG_INFO("Detected format: {}", std::string(models::toString(binary.binaryType)));
        
        if (!MagicDetector::isSupportedFormat(binary.binaryType)) {
            const std::string msg = "Unsupported binary format (not PE, ELF or MachO)" + binary.filename;
            MEDE_LOG_WARN("{}", msg);
            return common::Result<models::Sample>::failure(msg);
        }

        // Hash Calculation
        std::cout << "Calculating hashes..." << std::endl;
        auto hashResult = HashEngine::computeAll(binary.data);
        if (!hashResult) {
            MEDE_LOG_ERROR("Hash computation failed: {}", hashResult.error);
            return common::Result<models::Sample>::failure(hashResult.error);
        }
        const auto& hashes = hashResult.value;
        MEDE_LOG_DEBUG("SHA-256: {}", hashes.sha256);

        // Duplicate detection
        if (isDuplicate(hashes.sha256)) {
            const std::string msg = "Duplicate sample: SHA-256 already imported: " + hashes.sha256;
            MEDE_LOG_WARN("{}", msg);
            return common::Result<models::Sample>::failure(msg);
        }

        // Metadata extraction
        std::cout << "Extracting Metadata..." << std::endl;
        const auto id = nextId();
        auto metadata = MetadataExtractor::extract(binary, hashes, id);

        std::error_code ec;
        fs::create_directories(rawSampleDir(), ec);

        auto destPath = rawSampleDir() / binary.filename;
        if (fs::exists(destPath)) 
            destPath = rawSampleDir() / (std::to_string(id) + "_" + binary.filename);
        
        fs::copy_file(canonicalPath, destPath, fs::copy_options::overwrite_existing, ec);

        if (ec) {
            const std::string msg = "Failed to copy sample to storage: " + ec.message();
            MEDE_LOG_ERROR("{}", msg);
            return common::Result<models::Sample>::failure(msg);
        }

        // Build the final sample record
        models::Sample sample;
        sample.id = id;
        sample.storagePath = destPath.string();
        sample.hashes = hashes;
        sample.metadata = std::move(metadata);
        sample.status = models::AnalysisStatus::Imported;

        // Save Metadata
        std::cout << "Saving metadata..." << std::endl;
        MEDE_LOG_INFO("Saving metadata for sample ID={}", id);
        saveSampleMetadata(sample);

        MEDE_LOG_INFO("Import complete: ID={}, SHA256={}, Type={}, Size={} bytes", id, hashes.sha256, std::string(models::toString(binary.binaryType)), binary.filesize);
        std::cout << "Import Complete." << std::endl;

        return common::Result<models::Sample>::success(std::move(sample));
    }
}
