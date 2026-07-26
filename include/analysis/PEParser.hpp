#pragma once

#include <cstddef>
#include <span>
#include "models/PEInfo.hpp"

namespace mede::analysis {
    class PEParser {
        public:
            PEParser() = delete;

            [[nodiscard]] static models::PEInfo parse(std::span<const std::byte> data);
    };
}
