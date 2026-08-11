#pragma once

#include "models/ComparisonResult.hpp"
#include "models/FeatureVector.hpp"

namespace mede::diff {
    class HashComparator {
        public:
            HashComparator() = delete;

            [[nodiscard]] static models::HashDifference compare(const models::FeatureVector& a, const models::FeatureVector& b);
    };
}
