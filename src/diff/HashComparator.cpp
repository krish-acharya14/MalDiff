#include "diff/HashComparator.hpp"

namespace mede::diff {
    models::HashDifference HashComparator::compare(const models::FeatureVector& a, const models::FeatureVector& b) {
        models::HashDifference diff;

        diff.md5Before = a.hashes.md5;
        diff.md5After = b.hashes.md5;
        diff.md5Equal = (a.hashes.md5 == b.hashes.md5);

        diff.sha1Before = a.hashes.sha1;
        diff.sha1After = b.hashes.sha1;
        diff.sha1Equal = (a.hashes.sha1 == b.hashes.sha1);

        diff.sha256Before = a.hashes.sha256;
        diff.sha256After = b.hashes.sha256;
        diff.sha256Equal = (a.hashes.sha256 == b.hashes.sha256);

        return diff;
    }
}
