#include <gtest/gtest.h>

#include <nlohmann/json.hpp>

#include "models/AnalysisStatus.hpp"
#include "models/Sample.hpp"

using mede::models::AnalysisStatus;
using mede::models::HashInfo;
using mede::models::Sample;

namespace {

TEST(AnalysisStatusTest, RoundTripsThroughString) {
    EXPECT_EQ(mede::models::toString(AnalysisStatus::Analyzed), "Analyzed");
    EXPECT_EQ(mede::models::analysisStatusFromString("Analyzed"), AnalysisStatus::Analyzed);
    EXPECT_EQ(mede::models::analysisStatusFromString("garbage"), AnalysisStatus::Unknown);
}

TEST(HashInfoTest, EmptyDetectsAllFieldsUnset) {
    HashInfo h;
    EXPECT_TRUE(h.empty());

    h.sha256 = "abc123";
    EXPECT_FALSE(h.empty());
}

TEST(SampleTest, SerializesToAndFromJson) {
    Sample sample;
    sample.id = 42;
    sample.storagePath = "samples/raw/example.bin";
    sample.hashes.sha256 = "deadbeef";
    sample.metadata.originalFileName = "example.bin";
    sample.status = AnalysisStatus::Imported;

    nlohmann::json j = sample;
    ASSERT_EQ(j.at("id").get<mede::common::EntityId>(), 42u);
    ASSERT_EQ(j.at("status").get<std::string>(), "Imported");

    Sample roundTripped = j.get<Sample>();
    EXPECT_EQ(roundTripped.id, sample.id);
    EXPECT_EQ(roundTripped.storagePath, sample.storagePath);
    EXPECT_EQ(roundTripped.hashes.sha256, sample.hashes.sha256);
    EXPECT_EQ(roundTripped.status, AnalysisStatus::Imported);
}

}  
