#include "assert-utility.h"
#include <hearing-aid/HearingAidBuilder.h>
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
    std::vector<double> agcCrossFrequencies_;
    std::vector<double> agcCompressionRatios_;
    std::vector<double> agcKneepoints_;
    std::vector<double> agcKneepointGains_;
    std::vector<double> agcBroadbandOutputLimitingThresholds_;
    double agcAttack_{};
    double agcRelease_{};
    double firSampleRate_{};
    double iirSampleRate_{};
    double agcSampleRate_{};
    double agcFullScaleLevel_{};
    double feedbackGain_{};
    double filterEstimationForgettingFactor_{};
    double filterEstimationPowerThreshold_{};
    double filterEstimationStepSize_{};
    int firChannels_{};
    int iirChannels_{};
    int agcChannels_{};
    int firWindowSize_{};
    int firChunkSize_{};
    int iirChunkSize_{};
    int adaptiveFeedbackFilterLength_{};
    int signalWhiteningFilterLength_{};
    int persistentFeedbackFilterLength_{};
    int hardwareLatency_{};
    int saveQualityMetric_{};
    bool firInitialized_{};
    bool iirInitialized_{};
public:
    auto saveQualityMetric() const {
        return saveQualityMetric_;
    }

    auto hardwareLatency() const {
        return hardwareLatency_;
    }

    auto persistentFeedbackFilterLength() const {
        return persistentFeedbackFilterLength_;
    }

    auto signalWhiteningFilterLength() const {
        return signalWhiteningFilterLength_;
    }

    auto filterEstimationStepSize() const {
        return filterEstimationStepSize_;
    }

    auto adaptiveFeedbackFilterLength() const {
        return adaptiveFeedbackFilterLength_;
    }

    auto filterEstimationPowerThreshold() const {
        return filterEstimationPowerThreshold_;
    }

    auto filterEstimationForgettingFactor() const {
        return filterEstimationForgettingFactor_;
    }

    auto feedbackGain() const {
        return feedbackGain_;
    }

    auto agcBroadbandOutputLimitingThresholds() const {
        return agcBroadbandOutputLimitingThresholds_;
    }

    auto agcKneepointGains() const {
        return agcKneepointGains_;
    }

    auto agcKneepoints() const {
        return agcKneepoints_;
    }

    auto agcCompressionRatios() const {
        return agcCompressionRatios_;
    }

    auto agcCrossFrequencies() const {
        return agcCrossFrequencies_;
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

    auto agcFullScaleLevel() const {
        return agcFullScaleLevel_;
    }

    auto agcSampleRate() const {
        return agcSampleRate_;
    }

    auto agcRelease() const {
        return agcRelease_;
    }

    auto agcAttack() const {
        return agcAttack_;
    }

    auto agcChannels() const {
        return agcChannels_;
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
        filterEstimationForgettingFactor_ = p.filterEstimationForgettingFactor;
        filterEstimationPowerThreshold_ = p.filterEstimationPowerThreshold;
        filterEstimationStepSize_ = p.filterEstimationStepSize;
        signalWhiteningFilterLength_ = p.signalWhiteningFilterLength;
        persistentFeedbackFilterLength_ = p.persistentFeedbackFilterLength;
        hardwareLatency_ = p.hardwareLatency;
        saveQualityMetric_ = p.saveQualityMetric;
    }

    void initializeAutomaticGainControl(const AutomaticGainControl &p) override {
        agcCrossFrequencies_ = p.crossFrequencies;
        agcChannels_ = p.channels;
        agcAttack_ = p.attack;
        agcRelease_ = p.release;
        agcCompressionRatios_ = p.compressionRatios;
        agcKneepoints_ = p.kneepoints;
        agcKneepointGains_ = p.kneepointGains;
        agcBroadbandOutputLimitingThresholds_ = p.broadbandOutputLimitingThresholds;
        agcSampleRate_ = p.sampleRate;
        agcFullScaleLevel_ = p.fullScaleLevel;
    }
};

class HearingAidBuilderTests : public ::testing::Test {
    HearingAidInitializerStub initializer_;
    HearingAidBuilder builder{&initializer_};
    HearingAidBuilder::Parameters p{};
protected:
    void setFilterType(FilterType t) {
        setFilterType(name(t));
    }

    void setFilterType(std::string s) {
        p.filterType = std::move(s);
    }

    void build() {
        builder.build(p);
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

    void setCompressionRatios(std::vector<double> x) {
        p.compressionRatios = std::move(x);
    }

    void setKneepoints(std::vector<double> x) {
        p.kneepoints = std::move(x);
    }

    void setKneepointGains(std::vector<double> x) {
        p.kneepointGains = std::move(x);
    }

    void setBroadbandOutputLimitingThresholds(std::vector<double> x) {
        p.broadbandOutputLimitingThresholds = std::move(x);
    }

    void setAttack(double x) {
        p.attack = x;
    }

    void setRelease(double x) {
        p.release = x;
    }

    void assertFirCrossFrequencies(const std::vector<double> &x) {
        assertEqual(x, initializer_.firCrossFrequencies());
    }

    void assertIirCrossFrequencies(const std::vector<double> &x) {
        assertEqual(x, initializer_.iirCrossFrequencies());
    }

    void assertAgcCrossFrequencies(const std::vector<double> &x) {
        assertEqual(x, initializer_.agcCrossFrequencies());
    }

    void assertAgcCompressionRatios(const std::vector<double> &x) {
        assertEqual(x, initializer_.agcCompressionRatios());
    }

    void assertAgcKneepoints(const std::vector<double> &x) {
        assertEqual(x, initializer_.agcKneepoints());
    }

    void assertAgcKneepointGains(const std::vector<double> &x) {
        assertEqual(x, initializer_.agcKneepointGains());
    }

    void assertAgcBroadbandOutputLimitingThresholds(const std::vector<double> &x) {
        assertEqual(x, initializer_.agcBroadbandOutputLimitingThresholds());
    }

    void assertFirChannels(int n) {
        assertEqual(n, initializer_.firChannels());
    }

    void assertIirChannels(int n) {
        assertEqual(n, initializer_.iirChannels());
    }

    void assertAgcChannels(int n) {
        assertEqual(n, initializer_.agcChannels());
    }

    void assertAgcAttack(double x) {
        assertEqual(x, initializer_.agcAttack());
    }

    void assertAgcRelease(double x) {
        assertEqual(x, initializer_.agcRelease());
    }

    void assertAgcSampleRate(double x) {
        assertEqual(x, initializer_.agcSampleRate());
    }

    void assertAgcFullScaleLevel(double x) {
        assertEqual(x, initializer_.agcFullScaleLevel());
    }

    void setSampleRate(double r) {
        p.sampleRate = r;
    }

    void setFullScaleLevel(double r) {
        p.fullScaleLevel = r;
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
        setFeedback(Feedback::off);
    }

    void setFeedback() {
        setFeedback(Feedback::on);
    }

    void setFeedback(Feedback f) {
        p.feedback = name(f);
    }

    void setFeedbackGain(double x) {
        p.feedbackGain = x;
    }

    void setAdaptiveFeedbackFilterLength(int n) {
        p.adaptiveFeedbackFilterLength = n;
    }

    void setFilterEstimationForgettingFactor(double rho) {
        p.filterEstimationForgettingFactor = rho;
    }

    void setFilterEstimationPowerThreshold(double eps) {
        p.filterEstimationPowerThreshold = eps;
    }

    void setFilterEstimationStepSize(double mu) {
        p.filterEstimationStepSize = mu;
    }

    void setSignalWhiteningFilterLength(int wfl) {
        p.signalWhiteningFilterLength = wfl;
    }

    void setPersistentFeedbackFilterLength(int pfl) {
        p.persistentFeedbackFilterLength = pfl;
    }

    void setHardwareLatency(int hdl) {
        p.hardwareLatency = hdl;
    }

    void setSaveQualityMetric(int sqm) {
        p.saveQualityMetric = sqm;
    }

    void assertFilterEstimationForgettingFactor(double rho) {
        assertEqual(rho, initializer_.filterEstimationForgettingFactor());
    }

    void assertFilterEstimationPowerThreshold(double eps) {
        assertEqual(eps, initializer_.filterEstimationPowerThreshold());
    }

    void assertFilterEstimationStepSize(double mu) {
        assertEqual(mu, initializer_.filterEstimationStepSize());
    }

    void assertSignalWhiteningFilterLength(int wfl) {
        assertEqual(wfl, initializer_.signalWhiteningFilterLength());
    }

    void assertPersistentFeedbackFilterLength(int pfl) {
        assertEqual(pfl, initializer_.persistentFeedbackFilterLength());
    }

    void assertHardwareLatency(int hdl) {
        assertEqual(hdl, initializer_.hardwareLatency());
    }

    void assertSaveQualityMetric(int sqm) {
        assertEqual(sqm, initializer_.saveQualityMetric());
    }

    void assertFeedbackGain(double x) {
        assertEqual(x, initializer_.feedbackGain());
    }

    void assertAdaptiveFeedbackFilterLength(int x) {
        assertEqual(x, initializer_.adaptiveFeedbackFilterLength());
    }
};

TEST_F(HearingAidBuilderTests, firOnlyInitializesFir) {
    setFilterType(FilterType::fir);
    build();
    assertFirInitialized();
    assertIirNotInitialized();
}

TEST_F(HearingAidBuilderTests, firPassesParameters) {
    setFilterType(FilterType::fir);
    setCrossFrequencies({ 1, 2, 3 });
    setSampleRate(5);
    setWindowSize(6);
    setChunkSize(7);
    build();
    assertFirCrossFrequencies({ 1, 2, 3 });
    assertFirChannels(3+1);
    assertFirSampleRate(5);
    assertFirWindowSize(6);
    assertFirChunkSize(7);
}

TEST_F(HearingAidBuilderTests, iirOnlyInitializesIir) {
    setFilterType(FilterType::iir);
    build();
    assertIirInitialized();
    assertFirNotInitialized();
}

TEST_F(HearingAidBuilderTests, iirPassesParameters) {
    setFilterType(FilterType::iir);
    setCrossFrequencies({ 1, 2, 3 });
    setSampleRate(5);
    setChunkSize(6);
    build();
    assertIirCrossFrequencies({ 1, 2, 3 });
    assertIirChannels(3+1);
    assertIirSampleRate(5);
    assertIirChunkSize(6);
}

TEST_F(
    HearingAidBuilderTests,
    noFeedbackSetsGainAndAdaptiveFilterLengthToZero
) {
    setNoFeedback();
    build();
    assertFeedbackGain(0);
    assertAdaptiveFeedbackFilterLength(0);
}

TEST_F(
    HearingAidBuilderTests,
    feedbackPassesParameters
) {
    setFeedback();
    setFeedbackGain(1);
    setAdaptiveFeedbackFilterLength(2);
    setFilterEstimationForgettingFactor(3);
    setFilterEstimationPowerThreshold(4);
    setFilterEstimationStepSize(5);
    setSignalWhiteningFilterLength(6);
    setPersistentFeedbackFilterLength(7);
    setHardwareLatency(8);
    setSaveQualityMetric(9);
    build();
    assertFeedbackGain(1);
    assertAdaptiveFeedbackFilterLength(2);
    assertFilterEstimationForgettingFactor(3);
    assertFilterEstimationPowerThreshold(4);
    assertFilterEstimationStepSize(5);
    assertSignalWhiteningFilterLength(6);
    assertPersistentFeedbackFilterLength(7);
    assertHardwareLatency(8);
    assertSaveQualityMetric(9);
}

TEST_F(HearingAidBuilderTests, passesAgcParameters) {
    setCrossFrequencies({ 1, 2, 3 });
    setAttack(5);
    setRelease(6);
    setCompressionRatios({ 7, 8, 9 });
    setKneepoints({10, 11, 12});
    setKneepointGains({13, 14, 15});
    setBroadbandOutputLimitingThresholds({16, 17, 18});
    setSampleRate(19);
    setFullScaleLevel(20);
    build();
    assertAgcCrossFrequencies({ 1, 2, 3 });
    assertAgcChannels(3+1);
    assertAgcAttack(5);
    assertAgcRelease(6);
    assertAgcCompressionRatios({ 7, 8, 9 });
    assertAgcKneepoints({10, 11, 12});
    assertAgcKneepointGains({13, 14, 15});
    assertAgcBroadbandOutputLimitingThresholds({16, 17, 18});
    assertAgcSampleRate(19);
    assertAgcFullScaleLevel(20);
}
}}
