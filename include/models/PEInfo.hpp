#pragma once 

#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "models/Section.hpp"

namespace mede::models {
    struct PEInfo {
        bool valid{false};
        std::string parseError;

        struct FileHeader {
            std::uint16_t machine{0};
            std::uint16_t numberOfSections{0};
            std::uint32_t timeDateStamp{0};
            std::uint32_t characteristics{0};
        } fileHeader;

        struct OptionalHeader {
            std::uint16_t magic{0};
            bool is64Bit{false};
            std::uint64_t imageBase{0};
            std::uint32_t addressOfEntryPoint{0};
            std::uint32_t sectionAlignment{0};
            std::uint32_t fileAlignment {0};
            std::uint32_t sizeOfImage{0};
            std::uint32_t sizeOfHeaders{0};
            std::uint16_t subsystem{0};
            std::uint16_t dllCharacteristics{0};
            std::uint32_t numberOfRvaAndSizes{0};
        } optionalHeader;

        struct DataDirectory {
            std::uint32_t virtualAddress{0};
            std::uint32_t size{0};
        };
        std::vector<DataDirectory> dataDirectories;

        std::vector<Section> sections;
    };

    inline void to_json(nlohmann::json& j, const PEInfo& p) {
        j = nlohmann::json{
            {"valid", p.valid},
            {"parseError", p.parseError},
            {"fileHeader", {
                {"machine", p.fileHeader.machine},
                {"numberOfSections", p.fileHeader.numberOfSections},
                {"timeDateStamp", p.fileHeader.timeDateStamp},
                {"characteristics", p.fileHeader.characteristics},
            }},
            {"optionalHeader", {
                {"magic", p.optionalHeader.magic},
                {"is64Bit", p.optionalHeader.is64Bit},
                {"imageBase", p.optionalHeader.imageBase},
                {"addressOfEntryPoint", p.optionalHeader.addressOfEntryPoint},
                {"sectionAlignment", p.optionalHeader.sectionAlignment},
                {"fileAlignment", p.optionalHeader.fileAlignment},
                {"sizeOfImage", p.optionalHeader.sizeOfImage},
                {"sizeOfHeaders", p.optionalHeader.sizeOfHeaders},
                {"subsystem", p.optionalHeader.subsystem},
                {"dllCharacteristics", p.optionalHeader.dllCharacteristics},
                {"numberOfRvaAndSizes", p.optionalHeader.numberOfRvaAndSizes},
            }},
            {"sections", p.sections},
            {"dataDirectories", [&]() {
                nlohmann::json arr = nlohmann::json::array();
                for (const auto& dd : p.dataDirectories) {
                    arr.push_back({{"virtualAddress", dd.virtualAddress}, {"size", dd.size}});
                }
                return arr;
            }()},
        };
    }

    inline void from_json(const nlohmann::json& j, PEInfo& p) {
        p.valid = j.value("valid", false);
        p.parseError = j.value("parseError", "");

        if (j.contains("fileHeader")) {
            const auto& fh = j.at("fileHeader");
            p.fileHeader.machine = fh.value("machine", std::uint16_t{0});
            p.fileHeader.numberOfSections = fh.value("numberOfSections", std::uint16_t{0});
            p.fileHeader.timeDateStamp = fh.value("timeDateStamp", 0u);
            p.fileHeader.characteristics = fh.value("characteristics", 0u);
        }

        if (j.contains("optionalHeader")) {
            const auto& oh = j.at("optionalHeader");
            p.optionalHeader.magic = oh.value("magic", std::uint16_t{0});
            p.optionalHeader.is64Bit = oh.value("is64Bit", false);
            p.optionalHeader.imageBase = oh.value("imageBase", std::uint64_t{0});
            p.optionalHeader.addressOfEntryPoint = oh.value("addressOfEntryPoint", 0u);
            p.optionalHeader.sectionAlignment = oh.value("sectionAlignment", 0u);
            p.optionalHeader.fileAlignment = oh.value("fileAlignment", 0u);
            p.optionalHeader.sizeOfImage = oh.value("sizeOfImage", 0u);
            p.optionalHeader.sizeOfHeaders = oh.value("sizeOfHeaders", 0u);
            p.optionalHeader.subsystem = oh.value("subsystem", std::uint16_t{0});
            p.optionalHeader.dllCharacteristics = oh.value("dllCharacteristics", std::uint16_t{0});
            p.optionalHeader.numberOfRvaAndSizes = oh.value("numberOfRvaAndSizes", 0u);
        }

        if (j.contains("sections")) {
            p.sections = j.at("sections").get<std::vector<Section>>();
        }

        if (j.contains("dataDirectories")) {
            for (const auto& dd : j.at("dataDirectories")) {
                PEInfo::DataDirectory entry;
                entry.virtualAddress = dd.value("virtualAddress", 0u);
                entry.size = dd.value("size", 0u);
                p.dataDirectories.push_back(entry);
            }
        }
    }
}
