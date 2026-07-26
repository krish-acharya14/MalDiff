#pragma once

#include <cstddef>
#include <span>
#include <vector>
#include "models/ExtractedString.hpp"

namespace mede::analysis {
    class StringExtractor {
        public:
            StringExtractor() = delete;

            [[nodiscard]] static std::vector<models::ExtractedString> extractAscii(std::span<const std::byte> data, std::size_t minLength);

            [[nodiscard]] static std::vector<models::ExtractedString> extractUtf16Le(std::span<const std::byte> data, std::size_t minLength);

            [[nodiscard]] static std::vector<models::ExtractedString> extractAll(std::span<const std::byte> data, std::size_t minLength);
    };
}
