#include "pipeline/BinaryLoader.hpp"

#include <fstream>
#include <system_error>

namespace mede::pipeline {
    namespace fs = std::filesystem;

    common::Result<LoadedBinary> BinaryLoader::load(const fs::path& path) {
        std::error_code ec;

        if (!fs::is_regular_file(path, ec) || ec) 
            return common::Result<LoadedBinary>::failure("Not a regular file: " + path.string());
        
        const auto fileSize = fs::file_size(path, ec);
        if (ec) 
            return common::Result<LoadedBinary>::failure("Cannot determine file size: " + path.string() + ": " + ec.message());
        
        if (fileSize == 0) 
            return common::Result<LoadedBinary>::failure("File is empty: " + path.string());
        
        std::ifstream in(path, std::ios::in | std::ios::binary);
        if (!in.is_open()) 
            return common::Result<LoadedBinary>::failure("Cannot open file for reading: " + path.string());
        
        std::vector<uint8_t> data(fileSize);
        if (!in.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(fileSize)))
            return common::Result<LoadedBinary>::failure("Partial read from file: " + path.string());
        
        const auto absPath = fs::absolute(path, ec);
        const auto lastModTime = fs::last_write_time(path, ec);

        LoadedBinary binary;
        binary.absolutePath = ec ? path : absPath;
        binary.filename = path.filename().string();
        binary.extension = path.extension().string();
        binary.filesize = fileSize;
        binary.lastModified = lastModTime;
        binary.data = std::move(data);

        return common::Result<LoadedBinary>::success(std::move(binary));
    }
}
