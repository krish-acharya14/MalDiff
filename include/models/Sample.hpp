#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include "common/Types.hpp"
#include "models/AnalysisStatus.hpp"
#include "models/HashInfo.hpp"
#include "models/Metadata.hpp"

namespace mede::models {
    struct Sample {
        common::EntityId id{0};
        std::string storagePath;
        HashInfo hashes;
        Metadata metadata;
        AnalysisStatus status{AnalysisStatus::Unknown};
    };

    inline void to_json(nlohmann::json& j, const Sample& s) {
        j = nlohmann::json{
            {"id", s.id},
            {"storagePath", s.storagePath},
            {"hashes", s.hashes},
            {"metadata", s.metadata},
            {"status", std::string(toString(s.status))},
        };
    }

    inline void from_json(const nlohmann::json& j, Sample& s) {
        s.id = j.value("id", common::EntityId{0});
        s.storagePat = j.value("storagePath", "");
        if (j.contains("hashes")) s.hashes = j.at("hashes").get<HashInfo>();
        if (j.contains("metadata")) s.metadata = j.at("metadata").get<Metadata>();
        s.status = analysisStatusFromString(j.value("status", std::string("Unknown")));
    }
}
