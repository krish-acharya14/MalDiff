#pragma once

#include <filesystem>
#include "common/ErrorHandling.hpp"
#include "pipeline/LoadedBinary.hpp"

namespace mede::pipeline {
    class BinaryLoader {
        public:
            BinaryLoader() = delete;

            [[nodiscard]] static common::Result<LoadedBinary> load(const std::filesystem::path& path);
    };
}
