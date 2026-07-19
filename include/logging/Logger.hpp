#pragma once

#include <spdlog/spdlog.h>
#include <filesystem>
#include <string>
#include <memory>

namespace mede::logging {
    class Logger {
        public:
            struct Options {
                std::filesystem::path logDirectory{"logs"};
                std::string logFileName{"mede.log"};
                spdlog::level::level_enum level{spdlog::level::info};
                std::size_t maxFileSizeBytes{5 * 1024 * 1024};
                std::size_t maxFiles{3};
            };

            static void init(const Options& options, bool force = false);

            static void init(bool force = false);

            static std::shared_ptr<spdlog::logger>& get();

            static void shutdown();

        private:
            static std::shared_ptr<spdlog::logger> logger_;
            static bool initialized_;
    };
}

#define MEDE_LOG_TRACE(...) SPDLOG_LOGGER_TRACE(mede::logging::Logger::get(), __VA_ARGS__)
#define MEDE_LOG_DEBUG(...) SPDLOG_LOGGER_DEBUG(mede::logging::Logger::get(), __VA_ARGS__)
#define MEDE_LOG_INFO(...) SPDLOG_LOGGER_INFO(mede::logging::Logger::get(), __VA_ARGS__)
#define MEDE_LOG_WARN(...) SPDLOG_LOGGER_WARN(mede::logging::Logger::get(), __VA_ARGS__)
#define MEDE_LOG_ERROR(...) SPDLOG_LOGGER_ERROR(mede::logging::Logger::get(), __VA_ARGS__)
#define MEDE_LOG_CRITICAL(...) SPDLOG_LOGGER_CRITICAL(mede::logging::Logger::get(), __VA_ARGS__)
