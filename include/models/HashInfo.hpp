#pragma once

#include <nlohmann/json.hpp>
#include <string>

namespace mede::models {
    struct HashInfo {
        std::string md5;
        std::string sha1;
        std::string sha256;
        std::string ssdeep;

        [[nodiscard]] bool empty() const noexcept {
            return md5.empty() && sha1.empty() && sha256.empty() && ssdeep.empty();
        }
    };

    inline void to_json(nlohmann::json& j, const HashInfo& h) {
        j = nlohmann::json{
            {"md5", h.md5},
            {"sha1", h.sha1},
            {"sha256", h.sha256},
            {"ssdeep", h.ssdeep},
        };
    }

    inline void from_json(const nlohmann::json& j, HashInfo& h) {
        h.md5 = j.value("md5", "");
        h.sha1 = j.value("sha1", "");
        h.sha256 = j.value("sha256", "");
        h.ssdeep = j.value("ssdeep", "");
    }
}
