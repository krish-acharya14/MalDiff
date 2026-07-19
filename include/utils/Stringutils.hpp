#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace mede::utils {
    class StringUtils {
        public:
            StringUtils() = delete;

            static std::string tolower(std::string_view input);
            static std::string toUpper(std::string_view input);

            static std::string trim(std::string_view input);

            static std::vector<std::string> split(std::string_view input, char delimiter);

            static bool startsWith(std::string_view input, std::string_view prefix);
            static bool endsWith(std::string_view input, std::string_view suffix);
    };
}