#pragma once

#include "models/ComparisonResult.hpp"
#include "models/FeatureVector.hpp"

namespace mede::diff {
    class StringComparator {
        public:
            StringComparator() = delete;

            [[nodiscard]] static models::StringDifference compare(const models::FeatureVector& a, const models::FeatureVector& b);
    };
}
