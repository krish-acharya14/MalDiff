#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "models/Hashes.hpp"
#include "models/Import.hpp"
#include "models/Metadata.hpp"
#include "models/PEInfo.hpp"

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


    struct FeatureVector {
        Metadata metadata;
        Hashes hashes;
        std::string binaryFormat{"Unknown"};

        PEInfo pe;
        std::vector<Import> imports;
        std::size_t totalImportCount{0};

        std::vector<ExtractedString> strings;

        double overallEntropy{0.0};
    };

    inline void to_json(nlohmann::json& j, const FeatureVector& f) {
        j = nlohmann::json{
            {"metadata", f.metadata},
            {"hashes", f.hashes},
            {"binaryFormat", f.binaryFormat},
            {"pe", f.pe},
            {"imports", f.imports},
            {"totalImportCount", f.totalImportCount},
            {"strings", f.strings},
            {"overallEntropy", f.overallEntropy},
        };
    }

    inline void from_json(const nlohmann::json& j, FeatureVector& f) {
        if (j.contains("metadata")) f.metadata = j.at("metadata").get<Metadata>();
        if (j.contains("hashes")) f.hashes = j.at("hashes").get<Hashes>();
        f.binaryFormat = j.value("binaryFormat", std::string{"Unknown"});
        if (j.contains("pe")) f.pe = j.at("pe").get<PEInfo>();
        if (j.contains("imports")) f.imports = j.at("imports").get<std::vector<Import>>();
        f.totalImportCount = j.value("totalImportCount", std::size_t{0});
        if (j.contains("strings")) f.strings = j.at("strings").get<std::vector<ExtractedString>>();
        f.overallEntropy = j.value("overallEntropy", 0.0);
    }
}
