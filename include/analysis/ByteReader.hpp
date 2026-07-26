#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string>

namespace mede::analysis {
    class ByteReader {
        public:
            explicit ByteReader(std::span<const std::byte> data) noexcept : data_(data) {}

            [[nodiscard]] std::size_t size() const noexcept {return data_.size();}
            [[nodiscard]] std::span<const std::byte> data() const noexcept {return data_;}

            [[nodiscard]] bool inBounds(std::size_t offset, std::size_t length) const noexcept;

            [[nodiscard]] std::optional<std::uint8_t> readU8(std::size_t offset) const noexcept;
            [[nodiscard]] std::optional<std::uint16_t> readU16(std::size_t offset) const noexcept;
            [[nodiscard]] std::optional<std::uint32_t> readU32(std::size_t offset) const noexcept;
            [[nodiscard]] std::optional<std::uint64_t> readU64(std::size_t offset) const noexcept;

            [[nodiscard]] std::optional<std::string> readCstring(std::size_t offset, std::size_t maxLength) const noexcept;

        private:
            std::span<const std::byte> data_;
    };
}
