#include "analysis/PEParser.hpp"
#include "analysis/ByteReader.hpp"
#include "analysis/EntropyEngine.hpp"

namespace mede::analysis {
    models::PEInfo PEParser::parse(std::span<const std::byte> data) {
        models::PEInfo info;
        ByteReader reader(data);

        // DOS Header
        auto mz1 = reader.readU8(0);
        auto mz2 = reader.readU8(1);
        if (!mz1 || !mz2 || *mz1 != 'M' || *mz2 != 'Z') {
            info.parseError = "Invalid DOS header magic";
            return info;
        }

        auto e_lfanew = reader.readU32(0x3C);
        if (!e_lfanew) {
            info.parseError = "Invalid DOS header (truncated)";
            return info;
        }

        // NT Headers
        auto ntOffset = *e_lfanew;
        auto pe1 = reader.readU8(ntOffset);
        auto pe2 = reader.readU8(ntOffset + 1);
        auto pe3 = reader.readU8(ntOffset + 2);
        auto pe4 = reader.readU8(ntOffset + 3);
        if (!pe1 || !pe2 || !pe3 || !pe4 || *pe1 != 'P' || *pe2 != 'E' || *pe3 != 0 || *pe4 != 0) {
            info.parseError = "Invalid NT headers magic";
            return info;
        }

        // File Header
        auto fhOffset = ntOffset + 4;
        info.fileHeader.machine = reader.readU16(fhOffset).value_or(0);
        info.fileHeader.numberOfSections = reader.readU16(fhOffset + 2).value_or(0);
        info.fileHeader.timeDateStamp = reader.readU32(fhOffset + 4).value_or(0);
        auto sizeOfOptionalHeader = reader.readU16(fhOffset + 16).value_or(0);
        info.fileHeader.characteristics = reader.readU16(fhOffset + 18).value_or(0);

        // Optional Header
        auto ohOffset = fhOffset + 20;
        auto ohMagic = reader.readU16(ohOffset).value_or(0);
        info.optionalHeader.magic = ohMagic;
        info.optionalHeader.is64Bit = (ohMagic == 0x20B);

        info.optionalHeader.addressOfEntryPoint = reader.readU32(ohOffset + 16).value_or(0);
        
        if (info.optionalHeader.is64Bit) {
            info.optionalHeader.imageBase = reader.readU64(ohOffset + 24).value_or(0);
            info.optionalHeader.sectionAlignment = reader.readU32(ohOffset + 32).value_or(0);
            info.optionalHeader.fileAlignment = reader.readU32(ohOffset + 36).value_or(0);
            info.optionalHeader.sizeOfImage = reader.readU32(ohOffset + 56).value_or(0);
            info.optionalHeader.sizeOfHeaders = reader.readU32(ohOffset + 60).value_or(0);
            info.optionalHeader.subsystem = reader.readU16(ohOffset + 68).value_or(0);
            info.optionalHeader.dllCharacteristics = reader.readU16(ohOffset + 70).value_or(0);
            info.optionalHeader.numberOfRvaAndSizes = reader.readU32(ohOffset + 108).value_or(0);
        } else {
            info.optionalHeader.imageBase = reader.readU32(ohOffset + 28).value_or(0);
            info.optionalHeader.sectionAlignment = reader.readU32(ohOffset + 32).value_or(0);
            info.optionalHeader.fileAlignment = reader.readU32(ohOffset + 36).value_or(0);
            info.optionalHeader.sizeOfImage = reader.readU32(ohOffset + 56).value_or(0);
            info.optionalHeader.sizeOfHeaders = reader.readU32(ohOffset + 60).value_or(0);
            info.optionalHeader.subsystem = reader.readU16(ohOffset + 68).value_or(0);
            info.optionalHeader.dllCharacteristics = reader.readU16(ohOffset + 70).value_or(0);
            info.optionalHeader.numberOfRvaAndSizes = reader.readU32(ohOffset + 92).value_or(0);
        }

        auto dataDirOffset = info.optionalHeader.is64Bit ? (ohOffset + 112) : (ohOffset + 96);
        for (std::uint32_t i = 0; i < info.optionalHeader.numberOfRvaAndSizes && i < 16; ++i) {
            models::PEInfo::DataDirectory dd;
            dd.virtualAddress = reader.readU32(dataDirOffset + i * 8).value_or(0);
            dd.size = reader.readU32(dataDirOffset + i * 8 + 4).value_or(0);
            info.dataDirectories.push_back(dd);
        }

        // Sections
        auto secOffset = ohOffset + sizeOfOptionalHeader;
        for (std::uint16_t i = 0; i < info.fileHeader.numberOfSections; ++i) {
            auto currentSec = secOffset + i * 40;
            models::Section sec;
            auto nameOpt = reader.readCstring(currentSec, 8);
            sec.name = nameOpt.value_or("");
            sec.virtualSize = reader.readU32(currentSec + 8).value_or(0);
            sec.virtualAddress = reader.readU32(currentSec + 12).value_or(0);
            sec.rawSize = reader.readU32(currentSec + 16).value_or(0);
            sec.rawOffset = reader.readU32(currentSec + 20).value_or(0);
            sec.characteristics = reader.readU32(currentSec + 36).value_or(0);

            if (sec.rawSize > 0 && reader.inBounds(sec.rawOffset, sec.rawSize)) {
                auto secData = data.subspan(sec.rawOffset, sec.rawSize);
                sec.entropy = EntropyEngine::shannonEntropy(secData);
            }

            info.sections.push_back(sec);
        }

        info.valid = true;
        return info;
    }
}
