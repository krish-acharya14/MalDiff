#include "utils/StringUtils.hpp"

#include <algorithm>
#include <cctype>

namespace mede::utils {
    std::string StringUtils::tolower(std::string_view input) {
        std::string result(input);
        std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        });
        return result;
    }

    std::string StringUtils::toUpper(std::string_view input) {
        std::string result(input);
        std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
            return static_cast<char>(std::toupper(c));
        });
        return result;
    }

    std::string StringUtils::trim(std::string_view input) {
        const auto first = input.find_first_not_of(" \t\n\r\f\v");
        if (first == std::string_view::npos) {
            return "";
        }
        const auto last = input.find_last_not_of(" \t\n\r\f\v");
        return std::string(input.substr(first, last - first + 1));
    }

    std::vector<std::string> StringUtils::split(std::string_view input, char delimiter) {
        std::vector<std::string> tokens;
        std::size_t start = 0;

        while (start <= input.size()) {
            const auto pos = input.find(delimiter, start);
            if (pos == std::string_view::npos) {
                tokens.emplace_back(input.substr(start));
                break;
            }
            tokens.emplace_back(input.substr(start, pos - start));
            start = pos + 1;
        }

        return tokens;
    }

    bool StringUtils::startsWith(std::string_view input, std::string_view prefix) {
        return input.size() >= prefix.size() && input.substr(0, prefix.size()) == prefix;
    }

    bool StringUtils::endsWith(std::string_view input, std::string_view suffix) {
        return input.size() >= suffix.size() && input.substr(input.size() - suffix.size()) == suffix;
    }
}
