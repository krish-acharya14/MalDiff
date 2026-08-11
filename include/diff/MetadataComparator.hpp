#pragma once

#include "models/ComparisonResult.hpp"
#include "models/FeatureVector.hpp"

namespace mede::diff {
    class MetadataComparator {
        public:
            MetadataComparator() = delete;

            [[nodiscard]] static models::MetadataDifference compare(const models::FeatureVector& a, const models::FeatureVector& b);
    };
}
