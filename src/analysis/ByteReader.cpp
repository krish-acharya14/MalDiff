#include "analysis/ByteReader.hpp"
#include <cstring>
#include <cstdint>

namespace mede::analysis {
    bool ByteReader::inBounds(std::size_t offset, std::size_t length) const noexcept {
        if (offset > data_.size() || length > data_.size()) return false;
        return (offset + length) <= data_.size();
    }

    std::optional<std::uint8_t> ByteReader::readU8(std::size_t offset) const noexcept {
        if (!inBounds(offset, sizeof(std::uint8_t))) return std::nullopt;
        return std::to_integer<std::uint8_t>(data_[offset]);
    }

    std::optional<std::uint16_t> ByteReader::readU16(std::size_t offset) const noexcept {
        if (!inBounds(offset, sizeof(std::uint16_t))) return std::nullopt;
        std::uint16_t val;
        std::memcpy(&val, data_.data() + offset, sizeof(val));
        return val; // Assuming Little Endian architecture for simplicity as requested per phase 1 constraints
    }

    std::optional<std::uint32_t> ByteReader::readU32(std::size_t offset) const noexcept {
        if (!inBounds(offset, sizeof(std::uint32_t))) return std::nullopt;
        std::uint32_t val;
        std::memcpy(&val, data_.data() + offset, sizeof(val));
        return val;
    }

    std::optional<std::uint64_t> ByteReader::readU64(std::size_t offset) const noexcept {
        if (!inBounds(offset, sizeof(std::uint64_t))) return std::nullopt;
        std::uint64_t val;
        std::memcpy(&val, data_.data() + offset, sizeof(val));
        return val;
    }

    std::optional<std::string> ByteReader::readCstring(std::size_t offset, std::size_t maxLength) const noexcept {
        if (offset >= data_.size()) return std::nullopt;
        std::string res;
        for (std::size_t i = 0; i < maxLength && (offset + i) < data_.size(); ++i) {
            char c = static_cast<char>(std::to_integer<std::uint8_t>(data_[offset + i]));
            if (c == '\0') break;
            res += c;
        }
        return res;
    }
}
