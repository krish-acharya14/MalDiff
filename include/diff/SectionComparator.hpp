#pragma once

#include "models/ComparisonResult.hpp"
#include "models/FeatureVector.hpp"

namespace mede::diff {
    class SectionComparator {
        public:
            SectionComparator() = delete;

            [[nodiscard]] static models::SectionDifference compare(const models::FeatureVector& a, const models::FeatureVector& b);
    };
}
