#pragma once

#include "common/Types.hpp"
#include "models/HashInfo.hpp"
#include "models/Metadata.hpp"
#include "pipeline/LoadedBinary.hpp"

namespace mede::pipeline {
    class MetadataExtractor {
        public:
            MetadataExtractor() = delete;

            [[nodiscard]] static models::Metadata extract(const LoadedBinary& binary, const models::HashInfo& hashes, common::EntityId id);
    };
}
