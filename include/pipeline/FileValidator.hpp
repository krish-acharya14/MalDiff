#pragma once

#include <filesystem>
#include "common/ErrorHandling.hpp"

namespace mede::pipeline {
    class FileValidator {
        public:
            FileValidator() = delete;

            [[nodiscard]] static common::Result<std::filesystem::path> validate(const std::filesystem::path& path);
    };
}
