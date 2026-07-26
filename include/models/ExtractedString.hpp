#pragma once

#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>

namespace mede::models {
    struct ExtractedString {
        std::string value;
        std::uint64_t offset{0};
        std::string encoding;
    };

    inline void to_json(nlohmann::json& j, const ExtractedString& s) {
        j = nlohmann::json{
            {"value", s.value},
            {"offset", s.offset},
            {"encoding", s.encoding},
        };
    }

    inline void from_json(const nlohmann::json& j, ExtractedString& s) {
        s.value = j.value("value", "");
        s.offset = j.value("offset", std::uint64_t{0});
        s.encoding = j.value("encoding", "");
    }
}
