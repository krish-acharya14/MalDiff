#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include "models/FeatureVector.hpp"

namespace mede::models {
    struct Report {
        std::string version{"1.0"};
        std::string generatedAtIso8601;
        FeatureVector features;
    };

    inline void to_json(nlohmann::json& j, const Report& r) {
        j = nlohmann::json{
            {"version", r.version},
            {"generatedAtIso8601", r.generatedAtIso8601},
            {"features", r.features},
        };
    }

    inline void from_json(const nlohmann::json& j, Report& r) {
        r.version = j.value("version", "1.0");
        r.generatedAtIso8601 = j.value("generatedAtIso8601", "");
        if (j.contains("features")) r.features = j.at("features").get<FeatureVector>();
    }
}
