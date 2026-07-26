#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace mede::models {
    struct ImportedModule {
        std::string dllName;
        std::vector<std::string> functions;
    };

    inline void to_json(nlohmann::json& j, const ImportedModule& m) {
        j = nlohmann::json{
            {"dllName", m.dllName},
            {"functions", m.functions},
        };
    }

    inline void from_json(nlohmann::json& j, ImportedModule& m) {
        m.dllName = j.value("dllName", "");
        if (j.contains("functions")) 
            m.functions = j.at("functions").get<std::vector<std::string>>();
    }
}
