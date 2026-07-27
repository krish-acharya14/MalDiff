#pragma once

#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>

namespace mede::models {
    struct Section {
        std::string name;
        std::uint32_t virtualAddress{0};
        std::uint32_t virtualSize{0};
        std::uint32_t rawOffset{0};
        std::uint32_t rawSize{0};
        std::uint32_t characteristics{0};
        double entropy{0.0};
    };

    inline void to_json(nlohmann::json& j, const Section& s) {
        j = nlohmann::json{
            {"name", s.name},
            {"virtualAddress", s.virtualAddress},
            {"virtualSize", s.virtualSize},
            {"rawOffset", s.rawOffset},
            {"rawSize", s.rawSize},
            {"characteristics", s.characteristics},
            {"entropy", s.entropy},
        };
    }

    inline void from_json(const nlohmann::json& j, Section& s) {
        s.name = j.value("name", "");
        s.virtualAddress = j.value("virtualAddress", 0u);
        s.virtualSize = j.value("virtualSize", 0u);
        s.rawOffset = j.value("rawOffset", 0u);
        s.rawSize = j.value("rawSize", 0u);
        s.characteristics = j.value("characteristics", 0u);
        s.entropy = j.value("entropy", 0.0);
    }
}
