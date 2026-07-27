#include "analysis/ImportParser.hpp"
#include "analysis/ByteReader.hpp"

namespace mede::analysis {
    // Helper to map RVA to File Offset
    static std::uint32_t rvaToOffset(std::uint32_t rva, const std::vector<models::Section>& sections) {
        for (const auto& sec : sections) {
            if (rva >= sec.virtualAddress && rva < sec.virtualAddress + std::max(sec.virtualSize, sec.rawSize)) {
                return sec.rawOffset + (rva - sec.virtualAddress);
            }
        }
        return 0;
    }

    std::vector<models::Import> ImportParser::parse(std::span<const std::byte> data, const models::PEInfo& peInfo) {
        std::vector<models::Import> imports;
        if (!peInfo.valid || peInfo.dataDirectories.size() <= 1) return imports;

        auto importDirRva = peInfo.dataDirectories[1].virtualAddress;
        auto importDirSize = peInfo.dataDirectories[1].size;
        if (importDirRva == 0 || importDirSize == 0) return imports;

        auto importOffset = rvaToOffset(importDirRva, peInfo.sections);
        if (importOffset == 0) return imports;

        ByteReader reader(data);
        std::uint32_t currentDesc = importOffset;

        while (true) {
            auto originalFirstThunk = reader.readU32(currentDesc).value_or(0);
            auto nameRva = reader.readU32(currentDesc + 12).value_or(0);
            auto firstThunk = reader.readU32(currentDesc + 16).value_or(0);

            if (originalFirstThunk == 0 && firstThunk == 0) break;

            models::Import moduleImport;
            if (nameRva != 0) {
                auto nameOffset = rvaToOffset(nameRva, peInfo.sections);
                if (nameOffset != 0) {
                    moduleImport.dllName = reader.readCstring(nameOffset, 256).value_or("unknown");
                }
            }

            auto thunkRva = originalFirstThunk != 0 ? originalFirstThunk : firstThunk;
            auto thunkOffset = rvaToOffset(thunkRva, peInfo.sections);

            while (thunkOffset != 0) {
                std::uint64_t thunkData = 0;
                if (peInfo.optionalHeader.is64Bit) {
                    thunkData = reader.readU64(thunkOffset).value_or(0);
                    thunkOffset += 8;
                } else {
                    thunkData = reader.readU32(thunkOffset).value_or(0);
                    thunkOffset += 4;
                }

                if (thunkData == 0) break;

                // Check for ordinal
                bool isOrdinal = peInfo.optionalHeader.is64Bit ? (thunkData & 0x8000000000000000ULL) : (thunkData & 0x80000000);
                if (isOrdinal) {
                    moduleImport.functions.push_back("Ordinal_" + std::to_string(thunkData & 0xFFFF));
                } else {
                    // Function by name
                    auto nameDataRva = thunkData & 0x7FFFFFFF;
                    auto nameDataOffset = rvaToOffset(static_cast<std::uint32_t>(nameDataRva), peInfo.sections);
                    if (nameDataOffset != 0) {
                        // Skip hint (2 bytes)
                        auto funcName = reader.readCstring(nameDataOffset + 2, 256).value_or("");
                        if (!funcName.empty()) {
                            moduleImport.functions.push_back(funcName);
                        }
                    }
                }
            }
            if (!moduleImport.dllName.empty() || !moduleImport.functions.empty()) {
                imports.push_back(moduleImport);
            }

            currentDesc += 20;
        }
        return imports;
    }
}
