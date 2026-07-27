#pragma once

#include <cstddef>
#include <span>
#include <vector>
#include "models/Import.hpp"
#include "models/PEInfo.hpp"

namespace mede::analysis {
    class ImportParser {
        public:
            ImportParser() = delete;

            [[nodiscard]] static std::vector<models::Import> parse(std::span<const std::byte> data, const models::PEInfo& peInfo);
    };
}
