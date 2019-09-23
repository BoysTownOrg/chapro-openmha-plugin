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
    double firSampleRate_{};
    int firChannels_{};
    int firWindowSize_{};
    bool firInitialized_{};
    bool iirInitialized_{};
public:
    auto firWindowSize() const {
        return firWindowSize_;
    }
    
    auto firSampleRate() const {
        return firSampleRate_;
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
        firInitialized_ = true;
    }

    void initializeIirFilter() override {
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

    void assertFirChannels(int n) {
        assertEqual(n, initializer_.firChannels());
    }

    void setSampleRate(double r) {
        p.sampleRate = r;
    }

    void assertFirSampleRate(double r) {
        assertEqual(r, initializer_.firSampleRate());
    }

    void setWindowSize(int n) {
        p.windowSize = n;
    }

    void assertFirWindowSize(int n) {
        assertEqual(n, initializer_.firWindowSize());
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
    initialize();
    assertFirCrossFrequencies({ 1, 2, 3 });
    assertFirChannels(3+1);
    assertFirSampleRate(5);
    assertFirWindowSize(6);
}

TEST_F(HearingAidInitializationTests, iirOnlyInitializesIir) {
    setFilterType(FilterType::iir);
    initialize();
    assertIirInitialized();
    assertFirNotInitialized();
}
}}
