#include "diff/MetadataComparator.hpp"

namespace mede::diff {
    namespace {
        [[nodiscard]] std::string fileSizeOf(const models:: FeatureVector& f) {
            auto it = f.metadata.attributes.find("fileSize");
            return it != f.metadata.attributes.end() ? it->second : std::string{"unknown"};
        }

        [[nodiscard]] std::string architectureOf(const models::FeatureVector& f) {
            if (!f.pe.valid) return "unknown";
            return f.pe.optionalHeader.is64Bit ? "x64" : "x86";
        }

        [[nodiscard]] std::string timestampOf(const models::FeatureVector& f) {
            if (!f.pe.valid) return "unknown";
            return std::to_string(f.pe.fileHeader.timeDateStamp);
        }

        void addIfDifferent(std::vector<models::FieldChange>& changes, const std::string& field, const std::string& before, const std::string& after) {
            if (before != after)
                changes.push_back(models::FieldChange{field, before, after});
        }

        models::MetadataDifference MetadataComparator::compare(const models::FeatureVector& a, const models::FeatureVector& b) {
            models::MetadataDifference diff;

            addIfDifferent(diff.changes, "fileSize", fileSizeOf(a), fileSizeOf(b));
            addIfDifferent(diff.changes, "architecture", architectureOf(a), architectureOf(b));
            addIfDifferent(diff.changes, "timestampOf", timestampOf(a), timestampOf(b));
            addIfDifferent(diff.changes, "binaryFormat", a.binaryFormat, b.binaryFormat);

            return diff;
        }
    }
}
