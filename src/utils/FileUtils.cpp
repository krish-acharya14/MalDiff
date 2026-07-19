#include "utils/FileUtils.hpp"

#include <fstream>
#include <sstream>

namespace mede::utils {
    namespace fs = std::filesystem;

    std::optional<std::string> FileUtils::readTextFile(const fs::path& path) {
        std::ifstream in(path, std::ios::in | std::ios::binary);
        if (!in.is_open()) return std::nullopt;

        std::ostringstream buffer;
        buffer << in.rdbuf();
        return buffer.str();
    }

    bool FileUtils::writeTestFile(const fs::path& path, const std::string& content) {
        std::error_code ec;

        if (path.has_parent_path()) {
            fs::create_directories(path.parent_path(), ec);
            if (ec) return false;
        }

        std::ofstream out(path, std::ios::out | std::ios::binary | std::ios::trunc);
        if (!out.is_open()) return false;

        out << content;
        return static_cast<bool>(out);
    }

    bool FileUtils::isReadWritable(const fs::path& path) {
        std::error_code ec;
        const auto status = fs::status(path, ec);
        if (ec || !fs::exists(status)) return false;

        const auto perms = status.permissions();
        const bool readable = (perms & fs::perms::owner_read) != fs::perms::none;
        const bool writable = (perms & fs::perms::owner_write) != fs::perms::none;
        return readable && writable;
    }

    std::optional<std::uintmax_t> FileUtils::fileSize(const fs::path& path) {
        std::error_code ec;
        if (!fs::is_regular_file(path, ec) || ec) {
            return std::nullopt;
        }

        const auto size = fs::file_size(path, ec);
        if (ec) {
            return std::nullopt;
        }
        return size;
    }
}
