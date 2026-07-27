#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace mede::models {
    struct Import {
        std::string dllName;
        std::vector<std::string> functions;
    };

    inline void to_json(nlohmann::json& j, const Import& m) {
        j = nlohmann::json{
            {"dllName", m.dllName},
            {"functions", m.functions},
        };
    }

    inline void from_json(const nlohmann::json& j, Import& m) {
        m.dllName = j.value("dllName", "");
        if (j.contains("functions")) 
            m.functions = j.at("functions").get<std::vector<std::string>>();
    }
}
