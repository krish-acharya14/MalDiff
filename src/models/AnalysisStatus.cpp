#include "models/AnalysisStatus.hpp"

namespace mede::models {

    AnalysisStatus analysisStatusFromString(std::string_view value) noexcept {
        if (value == "Imported") return AnalysisStatus::Imported;

        if (value == "Queued") return AnalysisStatus::Queued;
        
        if (value == "Analyzing") return AnalysisStatus::Analyzing;
        
        if (value == "Analyzed") return AnalysisStatus::Analyzed;
        
        if (value == "Failed") return AnalysisStatus::Failed;
        
        if (value == "Quarantined") return AnalysisStatus::Quarantined;
        
        return AnalysisStatus::Unknown;
    }
}
