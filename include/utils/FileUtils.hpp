#pragma once

#include <filesystem>
#include <optional>
#include <string>

namespace mede::utils {
    class FileUtils {
        public:
            FileUtils() = delete;

            static std::optional<std::string> readTextFile(const std::filesystem::path& path);

            static bool writeTestFile(const std::filesystem::path& path, const std::string& content);

            static bool isReadWritable(const std::filesystem::path& path);

            static std::optional<std::uintmax_t> fileSize(const std::filesystem::path& path);
    };
}
