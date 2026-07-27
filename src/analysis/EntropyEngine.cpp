#include "analysis/EntropyEngine.hpp"
#include <cmath>
#include <array>
#include <cstdint>

namespace mede::analysis {
    double EntropyEngine::shannonEntropy(std::span<const std::byte> data) noexcept {
        if (data.empty()) return 0.0;
        std::array<std::size_t, 256> counts{};
        for (auto b : data) {
            counts[std::to_integer<std::uint8_t>(b)]++;
        }
        double entropy = 0.0;
        double size = static_cast<double>(data.size());
        for (auto count : counts) {
            if (count > 0) {
                double p = static_cast<double>(count) / size;
                entropy -= p * std::log2(p);
            }
        }
        return entropy;
    }
}
