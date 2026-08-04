#pragma once

#include "models/ComparisonResult.hpp"
#include "models/FeatureVector.hpp"

namespace mede::diff {
    class DiffEngine {
        public:
            DiffEngine() = delete;

            [[nodiscard]] static models::ComparisonResult compare(const models::FeatureVector& a, const models::FeatureVector& b);
    };
}
