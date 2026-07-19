#pragma once

#include <cstdint>
#include <filesystem>
#include <string>

namespace mede::common {
    namespace fs = std::filesystem;

    using EntityId = std::uint16_t;

    struct SchemaVersion {
        int major{1};
        int minor{0};

        [[nodiscard]] std::string toString() const {
            return std::to_string(major) + "." + std::to_string(minor);
        }
    };
}
