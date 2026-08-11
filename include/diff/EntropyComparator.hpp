#pragma once

#include "models/ComparisonResult.hpp"
#include "models/FeatureVector.hpp"

namespace mede::diff {
    class EntropyComparator {
        public:
            EntropyComparator() = delete;

            [[nodiscard]] static models::EntropyDifference compare(const models::FeatureVector& a, const models::FeatureVector& b);
    };
}
