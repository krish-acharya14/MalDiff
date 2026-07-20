#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>

#include "common/ErrorHandling.hpp"
#include "config/ConfigManager.hpp"

namespace fs = std::filesystem;
using mede::config::AppConfig;
using mede::config::ConfigManager;

namespace {

class ConfigManagerTest : public ::testing::Test {
  protected:
    void SetUp() override {
        testDir_ = fs::temp_directory_path() / "mede_test_config";
        fs::remove_all(testDir_);
        fs::create_directories(testDir_);
        configPath_ = testDir_ / "configs" / "config.json";
    }

    void TearDown() override { fs::remove_all(testDir_); }

    fs::path testDir_;
    fs::path configPath_;
};

TEST_F(ConfigManagerTest, GeneratesDefaultConfigWhenMissing) {
    ASSERT_FALSE(fs::exists(configPath_));

    ConfigManager manager(configPath_);
    const AppConfig cfg = manager.load();

    EXPECT_TRUE(fs::exists(configPath_));
    EXPECT_EQ(cfg.configVersion, "1.0");
    EXPECT_EQ(cfg.directories.samplesRaw, "samples/raw");
}

TEST_F(ConfigManagerTest, LoadsExistingConfig) {
    ConfigManager writer(configPath_);
    AppConfig custom = ConfigManager::makeDefault();
    custom.project.name = "custom-project";
    writer.save(custom);

    ConfigManager reader(configPath_);
    const AppConfig loaded = reader.load();

    EXPECT_EQ(loaded.project.name, "custom-project");
}

TEST_F(ConfigManagerTest, ThrowsOnMalformedJson) {
    fs::create_directories(configPath_.parent_path());
    std::ofstream out(configPath_);
    out << "{ this is not valid json ";
    out.close();

    ConfigManager manager(configPath_);
    EXPECT_THROW(manager.load(), mede::common::ConfigException);
}

}  
