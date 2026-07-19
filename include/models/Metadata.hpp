#pragma once

#include <nlohmann/json.hpp>
#include <chrono>
#include <string>
#include <unordered_map>
#include "common/Types.hpp"

namespace mede::models {
    struct Metadata {
        common::EntityId id{0};
        std::string originalFileName;
        std::string sourcePath;
        std::string importedAtIso8601;
        std::string notes;
        std::unordered_map<std::string, std::string> attributes;
    };

    inline void to_json(nlohmann::json& j, const Metadata& m) {
        j = nlohmann::json{
            {"id", m.id},
            {"originalFileName", m.originalFileName},
            {"sourcePath", m.sourcePath},
            {"importedAtIso8601", m.importedAtIso8601},
            {"notes", m.notes},
            {"attributes", m.attributes},
        };
    }

    inline void from_json(const nlohmann::json& j, Metadat& m) {
        m.id = j.value("id", common::EntityId{0});
        m.originalFileName = j.value("originalFileName", "");
        m.sourcePath = j.value("sourcePath", "");
        m.importedAtIso8601 = j.value("importedAtIso8601", "");
        m.notes = j.value("notes", "");
        if (j.contains("attributes"))
            m.attributes = j.at("attributes").get<std::unordered_map<std::string, std::string>>();
    }
}
