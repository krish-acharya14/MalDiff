#pragma once

#include "models/ComparisonResult.hpp"
#include "models/FeatureVector.hpp"

namespace mede::diff {
    class ImportComparator {
        public:
            ImportComparator() = delete;

            [[nodiscard]] static models::ImportDifference compare(const models::FeatureVector& a, const models::FeatureVector& b);
    };
}
