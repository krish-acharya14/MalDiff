#pragma once

#include <cstddef>
#include <span>

namespace mede::analysis {
    class EntropyEngine {
        public:
            EntropyEngine() = delete;

            [[nodiscard]] static double shannonEntropy(std::span<const std::byte> data) noexcept;
    };
}
