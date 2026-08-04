#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "models/Difference.hpp"
#include "models/Section.hpp"

namespace mede::models {
    using SectionDifference = Difference<Section>;

    // Metadata Difference
    struct MetadataDifference {
        std::vector<FieldChange> changes;

        [[nodiscard]] bool empty() const noexcept {return changes.empty();}
    };

    inline void to_json(nlohmann::json& j, const MetadataDifference& m) {
        j = nlohmann::json{{"changes", m.changes}};
    }

    inline void from_json(const nlohmann::json& j, MetadataDifference& m) {
        if (j.contains("changes")) m.changes = j.at("changes").get<std::vector<FieldChange>>();
    }

    // Import Difference
    struct ImportDifference {
        Difference<std::string> dlls;
        Difference<std::string> apis;

        [[nodiscard]] bool empty() const noexcept {return dlls.empty() && apis.empty();}
    };

    inline void to_json(nlohmann::json& j, const ImportDifference& i) {
        j = nlohmann::json{
            {"dlls", i.dlls},
            {"apis", i.apis},
        };
    }

    inline void from_json(const nlohmann::json& j, ImportDifference& i) {
        if (j.contains("dlls")) i.dlls = j.at("dlls").get<Difference<std::string>>();
        if (j.contains("apis")) i.apis = j.at("apis").get<Difference<std::string>>();
    }

    // Stings Difference
    struct StringDifference {
        Difference<std::string> asciiStrings;
        Difference<std::string> utf16Strings;

        [[nodiscard]] bool empty() const noexcept {
            return asciiStrings.empty() && utf16Strings.empty();
        }
    };

    inline void to_json(nlohmann::json& j, const StringDifference& s) {
        j = nlohmann::json{
            {"asciiStrings", s.asciiStrings},
            {"utf16Strings", s.utf16Strings},
        };
    }

    inline void from_json(const nlohmann::json& j, StringDifference& s) {
        if (j.contains("asciiStrings")) s.asciiStrings = j.at("asciiStrings").get<Difference<std::string>>();
        if (j.contains("utf16Strings")) s.utf16Strings = j.at("utf16Strings").get<Difference<std::string>>();
    }

    // Hash Difference
    struct HashDifference {
        bool md5Equal{true};
        bool sha1Equal{true};
        bool sha256Equal{true};

        std::string md5Before, md5After;
        std::string sha1Before, sha1After;
        std::string sha256Before, sha256After;

        [[nodiscard]] bool identical() const noexcept {
            return md5Equal && sha1Equal && sha256Equal;
        }
    };

    inline void to_json(nlohmann::json& j, const HashDifference& h) {
        j = nlohmann::json{
            {"md5Equal", h.md5Equal},
            {"sha1Equal", h.sha1Equal},
            {"sha256Equal", h.sha256Equal},
            {"md5Before", h.md5Before}, {"md5After", h.md5After},
            {"sha1Before", h.sha1Before}, {"sha1After", h.sha1After},
            {"sha256Before", h.sha256Before}, {"sha256After", h.sha256After},
        };
    }

    inline void from_json(const nlohmann::json& j, HashDifference& h) {
        h.md5Equal = j.value("md5Equal", true);
        h.sha1Equal = j.value("sha1Equal", true);
        h.sha256Equal = j.value("sha256Equal", true);
        h.md5Before = j.value("md5Before", "");
        h.md5After = j.value("md5After", "");
        h.sha1Before = j.value("sha1Before", "");
        h.sha1After = j.value("sha1After", "");
        h.sha256Before = j.value("sha256Before", "");
        h.sha256After = j.value("sha256After", "");
    }

    // Change in Entropy value
    struct EntropyValueChange {
        std::string label;
        double before{0.0};
        double after{0.0};
        double delta{0.0};
    };

    inline void to_json(nlohmann::json& j, const EntropyValueChange& e) {
        j = nlohmann::json{
            {"label", e.label},
            {"before", e.before},
            {"after", e.after},
            {"delta", e.delta},
        };
    }

    inline void from_json(const nlohmann::json& j, EntropyValueChange& e) {
        e.label = j.value("label", "");
        e.before = j.value("before", 0.0);
        e.after = j.value("after", 0.0);
        e.delta = j.value("delta", 0.0);
    }

    // Entropy Difference
    struct EntropyDifference {
        bool overallChanged{false};
        EntropyValueChange overall;
        std::vector<EntropyValueChange> sectionChanges;

        [[nodiscard]] bool empty() const noexcept {
            return !overallChanged && sectionChanges.empty();
        }
    };

    inline void to_json(nlohmann::json& j, const EntropyDifference& e) {
        j = nlohmann::json{
            {"overallChanged", e.overallChanged},
            {"overall", e.overall},
            {"sectionChanges", e.sectionChanges},
        };
    }

    inline void from_json(const nlohmann::json& j, EntropyDifference& e) {
        e.overallChanged = j.value("overallChanged", false);
        if (j.contains("overall")) e.overall = j.at("overall").get<EntropyValueChange>();
        if (j.contains("sectionChanges")) e.sectionChanges = j.at("sectionChanges").get<std::vector<EntropyValueChange>>();
    }

    // Final comparision Result
    struct ComparisonResult {
        MetadataDifference metadataDifference;
        SectionDifference sectionDifference;
        ImportDifference importDifference;
        StringDifference stringDifference;
        HashDifference hashDifference;
        EntropyDifference entropyDifference;
    };

    inline void to_json(nlohmann::json& j, const ComparisonResult& c) {
        j = nlohmann::json{
            {"metadataDifference", c.metadataDifference},
            {"sectionDifference", c.sectionDifference},
            {"importDifference", c.importDifference},
            {"stringDifference", c.stringDifference},
            {"hashDifference", c.hashDifference},
            {"entropyDifference", c.entropyDifference},
        };
    }

    inline void from_json(const nlohmann::json& j, ComparisonResult& c) {
        if (j.contains("metadataDifference")) c.metadataDifference = j.at("metadataDifference").get<MetadataDifference>();
        if (j.contains("sectionDifference")) c.sectionDifference = j.at("sectionDifference").get<SectionDifference>();
        if (j.contains("importDifference")) c.importDifference = j.at("importDifference").get<ImportDifference>();
        if (j.contains("stringDifference")) c.stringDifference = j.at("stringDifference").get<StringDifference>();
        if (j.contains("hashDifference")) c.hashDifference = j.at("hashDifference").get<HashDifference>();
        if (j.contains("entropyDifference")) c.entropyDifference = j.at("entropyDifference").get<EntropyDifference>();
    }
}
