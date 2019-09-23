#include "assert-utility.h"
#include <hearing-aid/HearingAidInitialization.h>
#include <gtest/gtest.h>
#include <string>

namespace hearing_aid { namespace {
void assertTrue(bool c) {
    EXPECT_TRUE(c);
}

void assertFalse(bool c) {
    EXPECT_FALSE(c);
}

class HearingAidInitializerStub : public HearingAidInitializer {
    std::vector<double> firCrossFrequencies_;
    std::vector<double> iirCrossFrequencies_;
    double firSampleRate_{};
    double iirSampleRate_{};
    int firChannels_{};
    int iirChannels_{};
    int firWindowSize_{};
    int firChunkSize_{};
    bool firInitialized_{};
    bool iirInitialized_{};
public:
    auto iirCrossFrequencies() const {
        return iirCrossFrequencies_;
    }

    auto firChunkSize() const {
        return firChunkSize_;
    }

    auto firWindowSize() const {
        return firWindowSize_;
    }

    auto firSampleRate() const {
        return firSampleRate_;
    }

    auto iirSampleRate() const {
        return iirSampleRate_;
    }

    auto iirChannels() const {
        return iirChannels_;
    }

    auto firChannels() const {
        return firChannels_;
    }

    auto firCrossFrequencies() const {
        return firCrossFrequencies_;
    }

    auto firInitialized() const {
        return firInitialized_;
    }

    auto iirInitialized() const {
        return iirInitialized_;
    }

    void initializeFirFilter(const FirParameters &p) override {
        firCrossFrequencies_ = p.crossFrequencies;
        firChannels_ = p.channels;
        firSampleRate_ = p.sampleRate;
        firWindowSize_ = p.windowSize;
        firChunkSize_ = p.chunkSize;
        firInitialized_ = true;
    }

    void initializeIirFilter(const IirParameters &p) override {
        iirCrossFrequencies_ = p.crossFrequencies;
        iirChannels_ = p.channels;
        iirSampleRate_ = p.sampleRate;
        iirInitialized_ = true;
    }
};

class HearingAidInitializationTests : public ::testing::Test {
    HearingAidInitializerStub initializer_;
    HearingAidInitialization initializer{&initializer_};
    HearingAidInitialization::Parameters p;
protected:
    void setFilterType(FilterType t) {
        setFilterType(name(t));
    }

    void setFilterType(std::string s) {
        p.filterType = std::move(s);
    }

    void initialize() {
        initializer.initialize(p);
    }

    bool firInitialized() {
        return initializer_.firInitialized();
    }

    bool iirInitialized() {
        return initializer_.iirInitialized();
    }

    void assertFirInitialized() {
        assertTrue(firInitialized());
    }

    void assertIirInitialized() {
        assertTrue(iirInitialized());
    }

    void assertIirNotInitialized() {
        assertFalse(iirInitialized());
    }

    void assertFirNotInitialized() {
        assertFalse(firInitialized());
    }

    void setCrossFrequencies(std::vector<double> x) {
        p.crossFrequencies = std::move(x);
    }

    void assertFirCrossFrequencies(const std::vector<double> &x) {
        assertEqual(x, initializer_.firCrossFrequencies());
    }

    void assertIirCrossFrequencies(const std::vector<double> &x) {
        assertEqual(x, initializer_.iirCrossFrequencies());
    }

    void assertFirChannels(int n) {
        assertEqual(n, initializer_.firChannels());
    }

    void assertIirChannels(int n) {
        assertEqual(n, initializer_.iirChannels());
    }

    void setSampleRate(double r) {
        p.sampleRate = r;
    }

    void assertFirSampleRate(double r) {
        assertEqual(r, initializer_.firSampleRate());
    }

    void assertIirSampleRate(double r) {
        assertEqual(r, initializer_.iirSampleRate());
    }

    void setWindowSize(int n) {
        p.windowSize = n;
    }

    void assertFirWindowSize(int n) {
        assertEqual(n, initializer_.firWindowSize());
    }

    void setChunkSize(int n) {
        p.chunkSize = n;
    }

    void assertFirChunkSize(int n) {
        assertEqual(n, initializer_.firChunkSize());
    }
};

TEST_F(HearingAidInitializationTests, firOnlyInitializesFir) {
    setFilterType(FilterType::fir);
    initialize();
    assertFirInitialized();
    assertIirNotInitialized();
}

TEST_F(HearingAidInitializationTests, firPassesParameters) {
    setFilterType(FilterType::fir);
    setCrossFrequencies({ 1, 2, 3 });
    setSampleRate(5);
    setWindowSize(6);
    setChunkSize(7);
    initialize();
    assertFirCrossFrequencies({ 1, 2, 3 });
    assertFirChannels(3+1);
    assertFirSampleRate(5);
    assertFirWindowSize(6);
    assertFirChunkSize(7);
}

TEST_F(HearingAidInitializationTests, iirOnlyInitializesIir) {
    setFilterType(FilterType::iir);
    initialize();
    assertIirInitialized();
    assertFirNotInitialized();
}

TEST_F(HearingAidInitializationTests, iirPassesParameters) {
    setFilterType(FilterType::iir);
    setCrossFrequencies({ 1, 2, 3 });
    setSampleRate(5);
    initialize();
    assertIirCrossFrequencies({ 1, 2, 3 });
    assertIirChannels(3+1);
    assertIirSampleRate(5);
}
}}
