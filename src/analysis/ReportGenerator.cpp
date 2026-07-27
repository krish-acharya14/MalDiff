#include "analysis/ReportGenerator.hpp"
#include "models/Report.hpp"
#include <fstream>
#include <chrono>
#include <sstream>
#include <iomanip>

namespace mede::analysis {
    nlohmann::json ReportGenerator::generate(const models::FeatureVector& features) {
        models::Report report;
        report.version = "1.0";
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::gmtime(&in_time_t), "%Y-%m-%dT%H:%M:%SZ");
        report.generatedAtIso8601 = ss.str();
        report.features = features;
        
        nlohmann::json j;
        to_json(j, report);
        return j;
    }

    void ReportGenerator::writeToFile(const nlohmann::json& report, const std::filesystem::path& path) {
        std::filesystem::create_directories(path.parent_path());
        std::ofstream out(path);
        if (out) {
            out << report.dump(4);
        }
    }
}
