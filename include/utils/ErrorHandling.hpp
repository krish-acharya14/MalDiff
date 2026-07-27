#pragma once

#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>

namespace mede::common {
    class MedeException : public std::runtime_error {
        public:
            explicit MedeException(std::string_view message) : std::runtime_error(std::string(message)) {}
    };

    class ConfigException : public MedeException {
        public: 
            explicit ConfigException(std::string_view message) : MedeException(message) {}
    };

    class StorageException : public MedeException {
        public: 
            explicit StorageException(std::string_view message) : MedeException(message) {}
    };

    class LoggingException : public MedeException {
        public:
            explicit LoggingException(std::string_view message) : MedeException(message) {}
    };

    class CLIException : public MedeException {
        public:
            explicit CLIException(std::string_view message) : MedeException(message) {}
    };

    template <typename T>
    struct Result {
        bool ok{false};
        T value{};
        std::string error{};

        static Result success(T val) {
            return Result{true, std::move(val), {}};
        }

        static Result failure(std::string err) {
            return Result{false, T{}, std::move(err)};
        }

        explicit operator bool() const noexcept { return ok; }
    };
}
