#include "logging/Logger.hpp"
// #include "common/LoggingException.hpp"

#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <stdexcept>

namespace mede::logging {
    std::shared_ptr<spdlog::logger> Logger::logger_ = nullptr;
    bool Logger::initialized_ = false;

    void Logger::init(const Options& options, bool force) {
        if (initialized_ && !force) return;
        
        try {
            std::filesystem::create_directories(options.logDirectory);

            const auto logFilePath = options.logDirectory / options.logFileName;

            auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            consoleSink -> set_level(options.level);

            auto fileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(logFilePath.string(), options.maxFileSizeBytes, options.maxFiles);
            fileSink -> set_level(spdlog::level::trace);

            std::vector<spdlog::sink_ptr> sinks{consoleSink, fileSink};
            auto newLogger = std::make_shared<spdlog::logger>("mede", sinks.begin(), sinks.end());
            newLogger -> set_level(options.level);
            newLogger -> flush_on(spdlog::level::warn); 
            newLogger -> set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%1%$] [%n] %v");

            spdlog::register_logger(newLogger);
            spdlog::set_default_logger(newLogger);

            logger_ = newLogger;
            initialized_ = true;
        } catch (const spdlog::spdlog_ex& ex) {
           throw std::runtime_error(std::string("Failed to initialize logger: ") + ex.what());
        }
    }

    void Logger::init(bool force) { init(Options{}, force); }

    std::shared_ptr<spdlog::logger>& Logger::get() {
        if (!logger_) 
            logger_ = spdlog::default_logger();
        
        return logger_;
    }

    void Logger::shutdown() {
        if (logger_) logger_ -> flush();

        spdlog::shutdown();
        logger_.reset();
        initialized_ = false;
    }
}
