#include <gtest/gtest.h>

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "common/Types.hpp"
#include "config/ConfigManager.hpp"
#include "models/BinaryType.hpp"
#include "pipeline/BinaryLoader.hpp"
#include "pipeline/FileValidator.hpp"
#include "pipeline/HashEngine.hpp"
#include "pipeline/MagicDetector.hpp"
#include "pipeline/MetadataExtractor.hpp"
#include "pipeline/SampleManager.hpp"

namespace fs = std::filesystem;
using mede::common::EntityId;
using mede::config::AppConfig;
using mede::config::ConfigManager;
using mede::models::AnalysisStatus;
using mede::models::BinaryType;
using mede::pipeline::BinaryLoader;
using mede::pipeline::FileValidator;
using mede::pipeline::HashEngine;
using mede::pipeline::MagicDetector;
using mede::pipeline::MetadataExtractor;
using mede::pipeline::SampleManager;

// ─── Test fixtures & helpers ────────────────────────────────────────────────

namespace {

// RAII temporary directory
class TempDir {
  public:
    TempDir() : path_(fs::temp_directory_path() / "mede_pipeline_test") {
        fs::remove_all(path_);
        fs::create_directories(path_);
    }
    ~TempDir() { fs::remove_all(path_); }

    const fs::path& path() const noexcept { return path_; }
    fs::path        file(const std::string& name) const { return path_ / name; }

  private:
    fs::path path_;
};

void writeBinary(const fs::path& p, const std::vector<uint8_t>& bytes) {
    std::ofstream out(p, std::ios::binary);
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    out.write(reinterpret_cast<const char*>(bytes.data()),
              static_cast<std::streamsize>(bytes.size()));
}

// Canonical magic bytes for each known format
const std::vector<uint8_t> kPeBytes{
    0x4D, 0x5A, 0x90, 0x00, 0x03, 0x00, 0x00, 0x00};  // MZ…
const std::vector<uint8_t> kElfBytes{
    0x7F, 0x45, 0x4C, 0x46, 0x02, 0x01, 0x01, 0x00};  // \x7fELF…
const std::vector<uint8_t> kMachOBytes{
    0xCF, 0xFA, 0xED, 0xFE, 0x07, 0x00, 0x00, 0x01};  // 64-bit LE
const std::vector<uint8_t> kUnknownBytes{
    0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0x01, 0x02, 0x03};

}  // namespace

// ─── FileValidator tests ─────────────────────────────────────────────────────

TEST(FileValidatorTest, RejectsNonExistentPath) {
    const auto r = FileValidator::validate("/nonexistent/definitely/missing.exe");
    EXPECT_FALSE(r.ok);
    EXPECT_FALSE(r.error.empty());
}

TEST(FileValidatorTest, RejectsEmptyFile) {
    TempDir tmp;
    const auto p = tmp.file("empty.bin");
    writeBinary(p, {});

    const auto r = FileValidator::validate(p);
    EXPECT_FALSE(r.ok);
}

TEST(FileValidatorTest, AcceptsValidNonEmptyFile) {
    TempDir tmp;
    const auto p = tmp.file("valid.bin");
    writeBinary(p, kPeBytes);

    const auto r = FileValidator::validate(p);
    EXPECT_TRUE(r.ok) << "Error: " << r.error;
}

TEST(FileValidatorTest, ReturnsCanonicalAbsolutePath) {
    TempDir tmp;
    const auto p = tmp.file("canon.bin");
    writeBinary(p, kPeBytes);

    const auto r = FileValidator::validate(p);
    ASSERT_TRUE(r.ok);
    EXPECT_TRUE(r.value.is_absolute());
}

// ─── BinaryLoader tests ──────────────────────────────────────────────────────

TEST(BinaryLoaderTest, LoadsAllBytesIntoMemory) {
    TempDir tmp;
    const auto p = tmp.file("sample.elf");
    writeBinary(p, kElfBytes);

    const auto r = BinaryLoader::load(p);
    ASSERT_TRUE(r.ok) << r.error;
    EXPECT_EQ(r.value.data, kElfBytes);
    EXPECT_EQ(r.value.filesize, kElfBytes.size());
}

TEST(BinaryLoaderTest, PopulatesFilenameAndExtension) {
    TempDir tmp;
    const auto p = tmp.file("binary.exe");
    writeBinary(p, kPeBytes);

    const auto r = BinaryLoader::load(p);
    ASSERT_TRUE(r.ok);
    EXPECT_EQ(r.value.filename, "binary.exe");
    EXPECT_EQ(r.value.extension, ".exe");
}

TEST(BinaryLoaderTest, PopulatesAbsolutePath) {
    TempDir tmp;
    const auto p = tmp.file("abs.bin");
    writeBinary(p, kElfBytes);

    const auto r = BinaryLoader::load(p);
    ASSERT_TRUE(r.ok);
    EXPECT_TRUE(r.value.absolutePath.is_absolute());
}

TEST(BinaryLoaderTest, RejectsMissingFile) {
    const auto r = BinaryLoader::load("/no/such/file.bin");
    EXPECT_FALSE(r.ok);
}

TEST(BinaryLoaderTest, RejectsEmptyFile) {
    TempDir tmp;
    const auto p = tmp.file("empty.bin");
    writeBinary(p, {});

    const auto r = BinaryLoader::load(p);
    EXPECT_FALSE(r.ok);
}

// ─── MagicDetector tests ─────────────────────────────────────────────────────

TEST(MagicDetectorTest, DetectsPE) {
    EXPECT_EQ(MagicDetector::detect(kPeBytes), BinaryType::PE);
}

TEST(MagicDetectorTest, DetectsELF) {
    EXPECT_EQ(MagicDetector::detect(kElfBytes), BinaryType::ELF);
}

TEST(MagicDetectorTest, DetectsMachO64BitLE) {
    EXPECT_EQ(MagicDetector::detect(kMachOBytes), BinaryType::MachO);
}

TEST(MagicDetectorTest, DetectsMachOFatBinary) {
    // CAFEBABE fat binary
    const std::vector<uint8_t> fatMachO{0xCA, 0xFE, 0xBA, 0xBE, 0x00, 0x00,
                                         0x00, 0x02};
    EXPECT_EQ(MagicDetector::detect(fatMachO), BinaryType::MachO);
}

TEST(MagicDetectorTest, ReturnsUnknownForArbitraryBytes) {
    EXPECT_EQ(MagicDetector::detect(kUnknownBytes), BinaryType::Unknown);
}

TEST(MagicDetectorTest, ReturnsUnknownForEmptySpan) {
    EXPECT_EQ(MagicDetector::detect(std::span<const uint8_t>{}),
              BinaryType::Unknown);
}

TEST(MagicDetectorTest, SupportedFormatsCoverPEELFMachO) {
    EXPECT_TRUE(MagicDetector::isSupportedFormat(BinaryType::PE));
    EXPECT_TRUE(MagicDetector::isSupportedFormat(BinaryType::ELF));
    EXPECT_TRUE(MagicDetector::isSupportedFormat(BinaryType::MachO));
    EXPECT_FALSE(MagicDetector::isSupportedFormat(BinaryType::Unknown));
}

// ─── HashEngine tests ─────────────────────────────────────────────────────────

TEST(HashEngineTest, EmptyBytesMd5KnownValue) {
    // MD5("") = d41d8cd98f00b204e9800998ecf8427e
    const auto r = HashEngine::computeAll(std::span<const uint8_t>{});
    ASSERT_TRUE(r.ok) << r.error;
    EXPECT_EQ(r.value.md5, "d41d8cd98f00b204e9800998ecf8427e");
}

TEST(HashEngineTest, EmptyBytesSha256KnownValue) {
    // SHA-256("") = e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
    const auto r = HashEngine::computeAll(std::span<const uint8_t>{});
    ASSERT_TRUE(r.ok);
    EXPECT_EQ(r.value.sha256,
              "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
}

TEST(HashEngineTest, EmptyBytesSha1KnownValue) {
    // SHA-1("") = da39a3ee5e6b4b0d3255bfef95601890afd80709
    const auto r = HashEngine::computeAll(std::span<const uint8_t>{});
    ASSERT_TRUE(r.ok);
    EXPECT_EQ(r.value.sha1, "da39a3ee5e6b4b0d3255bfef95601890afd80709");
}

TEST(HashEngineTest, PopulatesAllThreeFields) {
    const auto r = HashEngine::computeAll(kPeBytes);
    ASSERT_TRUE(r.ok);
    EXPECT_FALSE(r.value.md5.empty());
    EXPECT_FALSE(r.value.sha1.empty());
    EXPECT_FALSE(r.value.sha256.empty());
}

TEST(HashEngineTest, HashesAreConsistentAcrossCalls) {
    const auto r1 = HashEngine::computeAll(kPeBytes);
    const auto r2 = HashEngine::computeAll(kPeBytes);
    ASSERT_TRUE(r1.ok);
    ASSERT_TRUE(r2.ok);
    EXPECT_EQ(r1.value.md5,    r2.value.md5);
    EXPECT_EQ(r1.value.sha1,   r2.value.sha1);
    EXPECT_EQ(r1.value.sha256, r2.value.sha256);
}

TEST(HashEngineTest, DifferentDataProducesDifferentHashes) {
    const auto rPe  = HashEngine::computeAll(kPeBytes);
    const auto rElf = HashEngine::computeAll(kElfBytes);
    ASSERT_TRUE(rPe.ok);
    ASSERT_TRUE(rElf.ok);
    EXPECT_NE(rPe.value.sha256, rElf.value.sha256);
}

// ─── SampleManager integration tests ─────────────────────────────────────────

class SampleManagerTest : public ::testing::Test {
  protected:
    void SetUp() override {
        cfg_ = ConfigManager::makeDefault();
        // Ensure the managed directories exist
        fs::create_directories(tmp_.path() / cfg_.directories.samplesRaw);
        fs::create_directories(tmp_.path() / cfg_.directories.metadata);
    }

    fs::path writeSample(const std::string& name,
                         const std::vector<uint8_t>& bytes) {
        const auto p = tmp_.file(name);
        writeBinary(p, bytes);
        return p;
    }

    TempDir   tmp_;
    AppConfig cfg_;
};

TEST_F(SampleManagerTest, ImportSuccessfulPeSample) {
    const auto p = writeSample("sample.exe", kPeBytes);
    SampleManager mgr(cfg_, tmp_.path());

    const auto r = mgr.importSample(p);
    ASSERT_TRUE(r.ok) << r.error;
    EXPECT_GT(r.value.id, EntityId{0});
    EXPECT_FALSE(r.value.hashes.sha256.empty());
    EXPECT_FALSE(r.value.hashes.md5.empty());
    EXPECT_EQ(r.value.status, AnalysisStatus::Imported);
    EXPECT_FALSE(r.value.storagePath.empty());
}

TEST_F(SampleManagerTest, ImportSuccessfulElfSample) {
    const auto p = writeSample("sample.elf", kElfBytes);
    SampleManager mgr(cfg_, tmp_.path());

    const auto r = mgr.importSample(p);
    ASSERT_TRUE(r.ok) << r.error;
    EXPECT_EQ(r.value.metadata.attributes.at("binaryType"), "ELF");
}

TEST_F(SampleManagerTest, RejectsMissingFile) {
    SampleManager mgr(cfg_, tmp_.path());
    const auto r = mgr.importSample(tmp_.path() / "nonexistent.exe");
    EXPECT_FALSE(r.ok);
}

TEST_F(SampleManagerTest, RejectsEmptyFile) {
    const auto p = writeSample("empty.bin", {});
    SampleManager mgr(cfg_, tmp_.path());
    const auto r = mgr.importSample(p);
    EXPECT_FALSE(r.ok);
}

TEST_F(SampleManagerTest, RejectsUnknownBinaryFormat) {
    const auto p = writeSample("garbage.bin", kUnknownBytes);
    SampleManager mgr(cfg_, tmp_.path());
    const auto r = mgr.importSample(p);
    EXPECT_FALSE(r.ok);
}

TEST_F(SampleManagerTest, DetectsDuplicateBySha256) {
    const auto p1 = writeSample("a.exe", kPeBytes);
    const auto p2 = writeSample("b.exe", kPeBytes);  // identical content
    SampleManager mgr(cfg_, tmp_.path());

    const auto r1 = mgr.importSample(p1);
    ASSERT_TRUE(r1.ok) << r1.error;

    const auto r2 = mgr.importSample(p2);
    EXPECT_FALSE(r2.ok);
    EXPECT_TRUE(r2.error.find("Duplicate") != std::string::npos)
        << "Expected 'Duplicate' in: " << r2.error;
}

TEST_F(SampleManagerTest, MetadataJsonSavedCorrectly) {
    const auto p = writeSample("meta_check.exe", kPeBytes);
    SampleManager mgr(cfg_, tmp_.path());

    const auto r = mgr.importSample(p);
    ASSERT_TRUE(r.ok);

    const auto metaPath =
        tmp_.path() / cfg_.directories.metadata /
        (std::to_string(r.value.id) + ".json");
    ASSERT_TRUE(fs::exists(metaPath))
        << "Metadata file not found: " << metaPath;

    std::ifstream in(metaPath);
    const auto j = nlohmann::json::parse(in);
    EXPECT_EQ(j.at("id").get<EntityId>(), r.value.id);
    EXPECT_EQ(j["hashes"].at("sha256").get<std::string>(),
              r.value.hashes.sha256);
    EXPECT_EQ(j.at("status").get<std::string>(), "Imported");
}

TEST_F(SampleManagerTest, HashCorrectnessMatchesDirectComputation) {
    const auto p = writeSample("hash_check.elf", kElfBytes);
    SampleManager mgr(cfg_, tmp_.path());

    const auto r = mgr.importSample(p);
    ASSERT_TRUE(r.ok);

    const auto direct = HashEngine::computeAll(kElfBytes);
    ASSERT_TRUE(direct.ok);

    EXPECT_EQ(r.value.hashes.sha256, direct.value.sha256);
    EXPECT_EQ(r.value.hashes.md5,    direct.value.md5);
    EXPECT_EQ(r.value.hashes.sha1,   direct.value.sha1);
}

TEST_F(SampleManagerTest, MetadataContainsAllRequiredAttributes) {
    const auto p = writeSample("attrs.exe", kPeBytes);
    SampleManager mgr(cfg_, tmp_.path());

    const auto r = mgr.importSample(p);
    ASSERT_TRUE(r.ok);

    const auto& attrs = r.value.metadata.attributes;
    EXPECT_TRUE(attrs.count("fileSize")        > 0) << "Missing fileSize";
    EXPECT_TRUE(attrs.count("extension")       > 0) << "Missing extension";
    EXPECT_TRUE(attrs.count("lastModifiedUtc") > 0) << "Missing lastModifiedUtc";
    EXPECT_TRUE(attrs.count("magicBytesHex")   > 0) << "Missing magicBytesHex";
    EXPECT_TRUE(attrs.count("binaryType")      > 0) << "Missing binaryType";

    EXPECT_EQ(attrs.at("binaryType"), "PE");
    EXPECT_EQ(attrs.at("fileSize"),   std::to_string(kPeBytes.size()));
    EXPECT_FALSE(r.value.metadata.originalFileName.empty());
    EXPECT_FALSE(r.value.metadata.sourcePath.empty());
    EXPECT_FALSE(r.value.metadata.importedAtIso8601.empty());
}

TEST_F(SampleManagerTest, SequentialImportsGetIncrementingIds) {
    const auto p1 = writeSample("first.exe",  kPeBytes);
    const auto p2 = writeSample("second.elf", kElfBytes);
    SampleManager mgr(cfg_, tmp_.path());

    const auto r1 = mgr.importSample(p1);
    const auto r2 = mgr.importSample(p2);
    ASSERT_TRUE(r1.ok) << r1.error;
    ASSERT_TRUE(r2.ok) << r2.error;

    EXPECT_NE(r1.value.id, r2.value.id);
    EXPECT_GT(r2.value.id, r1.value.id);
}

TEST_F(SampleManagerTest, SampleCopiedToRawStorageDirectory) {
    const auto p = writeSample("stored.exe", kPeBytes);
    SampleManager mgr(cfg_, tmp_.path());

    const auto r = mgr.importSample(p);
    ASSERT_TRUE(r.ok);

    EXPECT_TRUE(fs::exists(r.value.storagePath))
        << "Stored file not found at: " << r.value.storagePath;
}
