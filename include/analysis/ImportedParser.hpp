#pragma once

#include <cstddef>
#include <span>
#include <vector>
#include "models/ImportedModule.hpp"
#include "models/PEInfo.hpp"

namespace mede::analysis {
    class ImportParser {
        public:
            ImportParser() = delete;

            [[nodiscard]] static std::vector<models::ImportedModule> parse(std::span<const std::byte> data, const models::PEInfo& peInfo);
    };
}
