#include "pipeline/FileValidator.hpp"

#include <filesystem>
#include <system_error>

namespace mede::pipeline {
    namespace fs = std::filesystem;

    common::Result<fs::path> FileValidator::validate(const fs::path& path) {
        std::error_code ec;

        if (!fs::exists(path, ec) || ec) 
            return common::Result<fs::path>::failure("File does not exist: " + path.string());
        
        if (!fs::is_regular_file(path, ec) || ec) 
            return common::Result<fs::path>::failure("Path is not a regular file: " + path.string());

        const auto status = fs::status(path, ec);
        if (ec) 
            return common::Result<fs::path>::failure("Cannot query file status: " + path.string() + ": " + ec.message());   
     
        const auto perms = status.permissions();
        if ((perms & fs::perms::owner_read) == fs::perms::none) 
            return common::Result<fs::path>::failure("File is not readable" + path.string());
        
        const auto size = fs::file_size(path, ec);
        if (ec)
            return common::Result<fs::path>::failure("Cannot determine file size: " + path.string() + ": " + ec.message());
        
        if (size == 0) 
            return common::Result<fs::path>::failure("File is empty " + path.string());
        
        std::error_code canonEc;
        auto canonical = fs::canonical(path, canonEc);
        if (canonEc) 
            return common::Result<fs::path>::failure("Cannot resolve canonical path: " + path.string() + ": " + canonEc.message());
        
        return common::Result<fs::path>::failure(std::move(canonical));
    }
}
