#include <gtest/gtest.h>

#include <filesystem>

#include "common/ErrorHandling.hpp"
#include "config/ConfigManager.hpp"
#include "storage/StorageManager.hpp"

namespace fs = std::filesystem;
using mede::config::AppConfig;
using mede::config::ConfigManager;
using mede::storage::StorageManager;

namespace {

class StorageManagerTest : public ::testing::Test {
  protected:
    void SetUp() override {
        testDir_ = fs::temp_directory_path() / "mede_test_storage";
        fs::remove_all(testDir_);
        fs::create_directories(testDir_);
        config_ = ConfigManager::makeDefault();
    }

    void TearDown() override { fs::remove_all(testDir_); }

    fs::path testDir_;
    AppConfig config_;
};

TEST_F(StorageManagerTest, CreatesAllManagedDirectories) {
    StorageManager manager(config_, testDir_);
    const auto created = manager.ensureDirectoryStructure();

    EXPECT_FALSE(created.empty());
    for (const auto& dir : manager.managedDirectories()) {
        EXPECT_TRUE(fs::exists(dir)) << "Missing directory: " << dir;
        EXPECT_TRUE(fs::is_directory(dir));
    }
}

TEST_F(StorageManagerTest, EnsureDirectoryStructureIsIdempotent) {
    StorageManager manager(config_, testDir_);
    manager.ensureDirectoryStructure();
    const auto secondPass = manager.ensureDirectoryStructure();

    EXPECT_TRUE(secondPass.empty());
}

TEST_F(StorageManagerTest, VerifyPermissionsSucceedsAfterCreation) {
    StorageManager manager(config_, testDir_);
    manager.ensureDirectoryStructure();

    EXPECT_NO_THROW(manager.verifyPermissions());
}

TEST_F(StorageManagerTest, VerifyPermissionsThrowsWhenDirectoryMissing) {
    StorageManager manager(config_, testDir_);
    EXPECT_THROW(manager.verifyPermissions(), mede::common::StorageException);
}

}  
