#pragma once

#include <string_view>

namespace mede::models {
    enum class BinaryType {
        Unknown = 0,
        PE,
        ELF,
        MachO,
    };

    [[nodiscard]] constexpr std::string_view toString(BinaryType type) noexcept {
        switch (type) {
            case BinaryType::PE: return "PE";
            case BinaryType::ELF: return "ELF";
            case BinaryType::MachO: return "MachO";
            default: return "Unknown";
        }
    }

    [[nodiscard]] inline BinaryType binaryTypeFromString(std::string_view s) noexcept {
        if (s == "PE") return BinaryType::PE;
        if (s == "ELF") return BinaryType::ELF;
        if (s == "MachO") return BinaryType::MachO;
        return BinaryType::Unknown;
    }
}