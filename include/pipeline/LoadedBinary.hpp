#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>
#include "models/BinaryType.hpp"

namespace mede::pipeline {
    struct LoadedBinary {
        std::filesystem::path absolutePath;
        std::string filename;
        std::string extension;
        std::uintmax_t filesize{0};
        std::filesystem::file_time_type lastModified;
        std::vector<uint8_t> data;
        models::BinaryType binaryType{models::BinaryType::Unknown};
    };
}
