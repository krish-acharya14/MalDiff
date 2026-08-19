#include "diff/ImportComparator.hpp"

#include <algorithm>
#include <set>

namespace mede::diff {
    [[nodiscard]] models::Difference<std::string> diffStringSets(const std::set<std::string>& a, const std::set<std::string>& b) {
        models::Difference<std::string> diff;

        std::set_difference(a.begin(), a.end(), b.begin(), b.end(), std::back_inserter(diff.removed));
        std::set_difference(b.begin(), b.end(), a.begin(), a.end(), std::back_inserter(diff.added));
        std::set_difference(a.begin(), a.end(), b.begin(), b.end(), std::back_inserter(diff.unchanged));

        return diff;
    }

    models::ImportDifference ImportComparator::compare(const models::FeatureVector& a, const models::FeatureVector& b) {
        models::ImportDifference diff;

        std::set<std::string> dllsA;
        std::set<std::string> apisA;
        for (const auto& imp : a.imports) {
            dllsA.insert(imp.dllName);
            for (const auto& fn : imp.functions)
                apisA.insert(imp.dllName + "!" + fn);
        }

        std::set<std::string> dllsB;
        std::set<std::string> apisB;
        for (const auto& imp : b.imports) {
            dllsB.insert(imp.dllName);
            for (const auto& fn : imp.functions)
                apisB.insert(imp.dllName + "!" + fn);
        }

        diff.dlls = diffStringSets(dllsA, dllsB);
        diff.apis = diffStringSets(apisA, apisB);

        return diff;
    }
}
