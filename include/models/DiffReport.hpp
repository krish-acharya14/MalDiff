#pragma once

#include <nlohmann/json.hpp>
#include <string>

#include "models/ComparisonResult.hpp"

namespace mede::models {
    struct DiffReport {
        std::string version{"1.0"};
        std::string generatedAtIso8601;
        std::string sampleAPath;
        std::string sampleBPath;
        ComparisonResult result;
    };

    inline void to_json(nlohmann::json& j, const DiffReport& r) {
        j = nlohmann::json{
            {"version", r.version},
            {"generatedAtIso8601", r.generatedAtIso8601},
            {"sampleAPath", r.sampleAPath},
            {"sampleBPath", r.sampleBPath},
            {"result", r.result},
        };
    }

    inline void from_json(const nlohmann::json& j, DiffReport& r) {
        r.version = j.value("version", "1.0");
        r.generatedAtIso8601 = j.value("generatedAtIso8601", "");
        r.sampleAPath = j.value("sampleAPath", "");
        r.sampleBPath = j.value("sampleBPath", "");
        if (j.contains("result")) r.result = j.at("result").get<ComparisonResult>();
    }
}
