#pragma once

#include <cstdint>
#include <span>
#include "models/BinaryType.hpp"

namespace mede::pipeline {
    class MagicDetector {
        public:
            MagicDetector() = delete;

            [[nodiscard]] static models::BinaryType detect(std::span<const uint8_t> bytes) noexcept;

            [[nodiscard]] static bool isSupportedFormat(models::BinaryType type) noexcept;
    };
}