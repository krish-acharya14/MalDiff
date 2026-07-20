#pragma once

#include <string>
#include <string_view>

namespace mede::models {
    enum class AnalysisStatus {
        Unknown = 0,
        Imported,
        Queued,
        Analyzing,
        Analyzed,
        Failed,
        Quarantined,
    };

    [[nodiscard]] constexpr std::string_view toString(AnalysisStatus status) noexcept {
        switch (status) {
            case AnalysisStatus::Unknown:
                return "Unknown";
            case AnalysisStatus::Imported:
                return "Imported";
            case AnalysisStatus::Queued:
                return "Queued";
            case AnalysisStatus::Analyzing:
                return "Analyzing";
            case AnalysisStatus::Analyzed:
                return "Analyzed";
            case AnalysisStatus::Failed:
                return "Failed";
            case AnalysisStatus::Quarantined:
                return "Quarantined";
        }
        return "Unknown";
    }

    [[nodiscard]] AnalysisStatus analysisStatusFromString(std::string_view value) noexcept;
}
