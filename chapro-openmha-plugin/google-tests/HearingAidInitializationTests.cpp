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
    double feedbackGain_{};
    int firChannels_{};
    int iirChannels_{};
    int firWindowSize_{};
    int firChunkSize_{};
    int iirChunkSize_{};
    int adaptiveFeedbackFilterLength_{};
    bool firInitialized_{};
    bool iirInitialized_{};
public:
    auto adaptiveFeedbackFilterLength() const {
        return adaptiveFeedbackFilterLength_;
    }

    auto feedbackGain() const {
        return feedbackGain_;
    }

    auto iirCrossFrequencies() const {
        return iirCrossFrequencies_;
    }

    auto iirChunkSize() const {
        return iirChunkSize_;
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
        iirChunkSize_ = p.chunkSize;
        iirInitialized_ = true;
    }

    void initializeFeedbackManagement(const FeedbackManagement &p) override {
        feedbackGain_ = p.gain;
        adaptiveFeedbackFilterLength_ = p.adaptiveFilterLength;
    }
};

class HearingAidInitializationTests : public ::testing::Test {
    HearingAidInitializerStub initializer_;
    HearingAidInitialization initializer{&initializer_};
    HearingAidInitialization::Parameters p{};
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

    void assertIirChunkSize(int n) {
        assertEqual(n, initializer_.iirChunkSize());
    }

    void setNoFeedback() {
        p.feedback = "no";
    }

    void setFeedback() {
        p.feedback = "yes";
    }

    void setFeedbackGain(double x) {
        p.feedbackGain = x;
    }

    void setAdaptiveFeedbackFilterLength(int n) {
        p.adaptiveFeedbackFilterLength = n;
    }

    void assertFeedbackGain(double x) {
        assertEqual(x, initializer_.feedbackGain());
    }

    void assertAdaptiveFeedbackFilterLength(int x) {
        assertEqual(x, initializer_.adaptiveFeedbackFilterLength());
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
    setChunkSize(6);
    initialize();
    assertIirCrossFrequencies({ 1, 2, 3 });
    assertIirChannels(3+1);
    assertIirSampleRate(5);
    assertIirChunkSize(6);
}

TEST_F(
    HearingAidInitializationTests,
    noFeedbackSetsGainAndAdaptiveFilterLengthToZero
) {
    setNoFeedback();
    initialize();
    assertFeedbackGain(0);
    assertAdaptiveFeedbackFilterLength(0);
}

TEST_F(
    HearingAidInitializationTests,
    feedbackPassesParameters
) {
    setFeedback();
    setFeedbackGain(1);
    setAdaptiveFeedbackFilterLength(2);
    initialize();
    assertFeedbackGain(1);
    assertAdaptiveFeedbackFilterLength(2);
}
}}
