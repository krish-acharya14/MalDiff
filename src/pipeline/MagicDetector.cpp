#include "pipeline/MagicDetector.hpp"

namespace mede::pipeline {
    models::BinaryType MagicDetector::detect(std::span<const uint8_t> bytes) noexcept {
        if (bytes.size() < 2) return models::BinaryType::Unknown;

        if (bytes[0] == 0x4Du && bytes[1] == 0x5Au) 
            return models::BinaryType::PE;

        if (bytes.size() >= 4 && bytes[0] == 0x7Fu && bytes[1] == 0x45u && bytes[2] == 0x4Cu && bytes[3] == 0x46u)
            return models::BinaryType::ELF;

        if (bytes.size() >= 4) {
            const uint32_t magic = (static_cast<uint32_t>(bytes[0]) << 24u) | (static_cast<uint32_t>(bytes[1]) << 16u) | (static_cast<uint32_t>(bytes[2]) <<  8u) | (static_cast<uint32_t>(bytes[3]));

            switch (magic) {
                case 0xFEEDFACEu:  // 32-bit BE
                case 0xCEFAEDFEu:  // 32-bit LE
                case 0xFEEDFACFu:  // 64-bit BE
                case 0xCFFAEDFEu:  // 64-bit LE
                case 0xCAFEBABEu:  // fat binary (also used by Java; context matters)
                case 0xBEBAFECAu:  // fat LE
                    return models::BinaryType::MachO;
                default:
                    break;
            }
        }

        return models::BinaryType::Unknown;
    }

    bool MagicDetector::isSupportedFormat(models::BinaryType type) noexcept {
        return type == models::BinaryType::PE || type == models::BinaryType::ELF || type == models::BinaryType::MachO;
    }
}
