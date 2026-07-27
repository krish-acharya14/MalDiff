#include "analysis/StringExtractor.hpp"
#include <cctype>
#include <cstdint>

namespace mede::analysis {
    std::vector<models::ExtractedString> StringExtractor::extractAscii(std::span<const std::byte> data, std::size_t minLength) {
        std::vector<models::ExtractedString> results;
        std::string current;
        std::uint64_t currentOffset = 0;
        for (std::size_t i = 0; i < data.size(); ++i) {
            char c = static_cast<char>(std::to_integer<std::uint8_t>(data[i]));
            if (std::isprint(c)) {
                if (current.empty()) currentOffset = i;
                current += c;
            } else {
                if (current.length() >= minLength) {
                    results.push_back({current, currentOffset, "ASCII"});
                }
                current.clear();
            }
        }
        if (current.length() >= minLength) {
            results.push_back({current, currentOffset, "ASCII"});
        }
        return results;
    }

    std::vector<models::ExtractedString> StringExtractor::extractUtf16Le(std::span<const std::byte> data, std::size_t minLength) {
        std::vector<models::ExtractedString> results;
        std::string current;
        std::uint64_t currentOffset = 0;
        for (std::size_t i = 0; i + 1 < data.size(); i += 2) {
            auto low = std::to_integer<std::uint8_t>(data[i]);
            auto high = std::to_integer<std::uint8_t>(data[i+1]);
            if (high == 0 && std::isprint(low)) {
                if (current.empty()) currentOffset = i;
                current += static_cast<char>(low);
            } else {
                if (current.length() >= minLength) {
                    results.push_back({current, currentOffset, "UTF-16LE"});
                }
                current.clear();
            }
        }
        if (current.length() >= minLength) {
            results.push_back({current, currentOffset, "UTF-16LE"});
        }
        return results;
    }

    std::vector<models::ExtractedString> StringExtractor::extractAll(std::span<const std::byte> data, std::size_t minLength) {
        auto ascii = extractAscii(data, minLength);
        auto utf16 = extractUtf16Le(data, minLength);
        ascii.insert(ascii.end(), std::make_move_iterator(utf16.begin()), std::make_move_iterator(utf16.end()));
        return ascii;
    }
}
