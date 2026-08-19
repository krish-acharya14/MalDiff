#include "diff/EntropyComparator.hpp"

#include <cmath>
#include <unordered_map>

namespace mede::diff {
    namespace {
        constexpr double kEpsilon = 1e-9;

        [[nodiscard]] bool changed(double before, double after) noexcept {
            return std::fabs(before - after) > kEpsilon;
        }
    }

    models::EntropyDifference EntropyComparator::compare(const models::FeatureVector& a, const models::FeatureVector& b) {
        models::EntropyDifference diff;

        if (changed(a.overallEntropy, b.overallEntropy)) {
            diff.overallChanged = true;
            diff.overall = models::EntropyValueChange{"overall", a.overallEntropy, b.overallEntropy, b.overallEntropy - a.overallEntropy};
        }

        std::unordered_map<std::string, double> entropyByNameB;
        for (const auto& section : b.pe.sections) 
            entropyByNameB[section.name] = section.entropy;
        
        for (const auto& sectionA : a.pe.sections) {
            auto it = entropyByNameB.find(sectionA.name);
            if (it == entropyByNameB.end()) continue;

            if (changed(sectionA.entropy, it->second))
                diff.sectionChanges.push_back(models::EntropyValueChange{sectionA.name, sectionA.entropy, it->second, it->second - sectionA.entropy});
        }

        return diff;
    }
}
