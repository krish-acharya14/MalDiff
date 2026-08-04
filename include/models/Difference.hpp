#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace mede::models {
    template <typename T>
    struct Modification {
        T before{};
        T after{};
    };

    template <typename T>
    inline void to_json(nlohmann::json& j, const Modification<T>& m) {
        j = nlohmann::json{
            {"before", m.before},
            {"after", m.after},
        };
    }

    template <typename T>
    inline void from_json(nlohmann::json& j, Modification<T>& m) {
        if (j.contains("before"))
            m.before = j.at("before").get<T>();
        
        if (j.contains("after"))
            m.after = j.at("after").get<T>();
    }

    template <typename T>
    struct Difference {
        std::vector<T> added;
        std::vector<T> removed;
        std::vector<Modification<T>> modified;
        std::vector<T> unchanged;

        [[nodiscard]] bool empty() const noexcept {
            return added.empty() && removed.empty() && modified.empty() && unchanged.empty();
        }

        [[nodiscard]] bool hasChanged() const noexcept {
            return !added.empty() || !removed.emtpy() || !modified.empty();
        }
    };

    template <typename T>
    inline void to_json(nlohmann::json& j, const Difference<T>& d) {
        j = nlohmann::json{
            {"added", d.added},
            {"removed", d.removed},
            {"modified", d.modified},
            {"unchanged", d.unchanged},
        };
    }

    template <typename T>
    inline void from_json(const nlohmann::json& j, Difference<T>& d) {
        if (j.contains("added")) d.added = j.at("added").get<std::vector<T>>();
        if (j.contains("removed")) d.removed = j.at("removed").get<std::vector<T>>();
        if (j.contains("modified")) d.modified = j.at("modified").get<std::vector<Modification<T>>>();
        if (j.contains("unchanged")) d.unchanged = j.at("unchanged").get<std::vector<T>>();
    }

    struct FieldChange {
        std::string field;
        std::string before;
        std::string after;
    };

    inline void to_json(nlohmann::json& j, const FieldChange& f) {
        j = nlohmann::json{
            {"field", f.field},
            {"before", f.before},
            {"after", f.after},
        };
    }

    inline void from_json(const nlohmann::json& j, FieldChange& f) {
        f.field = j.value("field", "");
        f.before = j.value("before", "");
        f.after = j.value("after", "");
    }
    
}
