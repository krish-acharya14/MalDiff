#include "pipeline/MetadataExtractor.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace mede::pipeline {
    namespace {
        std::string formatIso8601(std::chrono::system_clock::time_point tp) {
            const std::time_t t = std::chrono::system_clock::to_time_t(tp);
            std::tm utcTm{};

        #ifdef _WIN32
            gmtime_s(&utcTm, &t);
        #else
            gmtime_r(&t, &utcTm);
        #endif
            char buf[32];
            std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", &utcTm);
            return std::string(buf);
        }

        std::string nowIso8601() {
            return formatIso8601(std::chrono::system_clock::now());
        }

        std::string fileTimeToIso8601(std::filesystem::file_time_type ft) {
            const auto systemTp = std::chrono::file_clock::to_sys(ft);
            return formatIso8601(systemTp);
        }

        std::string magicBytesHex(const std::vector<uint8_t>& data) {
            const std::size_t count = std::min(data.size(), std::size_t{8});
            std::ostringstream oss;
            oss << std::hex << std::setfill('0');

            for (std::size_t i = 0; i < count; ++i) 
                oss << std::setw(2) << static_cast<int>(data[i]);

            return oss.str();
        }
    }

    models::Metadata MetadataExtractor::extract(const LoadedBinary& binary, const models::HashInfo& hashes, common::EntityId id) {
        models::Metadata meta;
        meta.id = id;
        meta.originalFileName = binary.filename;
        meta.sourcePath = binary.absolutePath;
        meta.importedAtIso8601 = nowIso8601();

        meta.attributes["fileSize"] = std::to_string(binary.filesize);
        meta.attributes["extension"] = binary.extension;
        meta.attributes["lastModifiedUtc"] = fileTimeToIso8601(binary.lastModified);
        meta.attributes["magicBytesHex"] = magicBytesHex(binary.data);
        meta.attributes["binaryType"] = std::string(models::toString(binary.binaryType));

        return meta;
    }
}
