#pragma once

#include <cstdint>
#include <cstddef>
#include <span>
#include <string>
#include "common/ErrorHandling.hpp"
#include "models/HashInfo.hpp"

namespace mede::pipeline {
    class HashEngine {
        public:
            HashEngine() = delete;

            [[nodiscard]] static common::Result<models::HashInfo> computeAll(std::span<const uint8_t> data);

            [[nodiscard]] static std::string toHexString(const unsigned char* bytes, std::size_t len);
    };
}
