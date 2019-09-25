#include "HearingAidBuilder.h"
#include <gsl/gsl>

namespace hearing_aid {
void HearingAidBuilder::build(const Parameters &p) {
    prepareFilter(p);
    prepareFeedbackManagement(p);
    prepareAutomaticGainControl(p);
}

void HearingAidBuilder::prepareFilter(const Parameters &p) {
    if (p.filterType == name(FilterType::fir))
        buildFirFilter(p);
    else
        buildIirFilter(p);
}

void HearingAidBuilder::buildFirFilter(const Parameters &p) {
    HearingAidInitializer::FirParameters firParameters;
    firParameters.crossFrequencies = p.crossFrequencies;
    firParameters.channels = channels(p);
    firParameters.sampleRate = p.sampleRate;
    firParameters.windowSize = p.windowSize;
    firParameters.chunkSize = p.chunkSize;
    initializer->initializeFirFilter(firParameters);
    filter_ = filterFactory->makeFir();
}

int HearingAidBuilder::channels(const Parameters &p) {
    return gsl::narrow<int>(p.crossFrequencies.size() + 1);
}

void HearingAidBuilder::buildIirFilter(const Parameters &p) {
    HearingAidInitializer::IirParameters iirParameters;
    iirParameters.crossFrequencies = p.crossFrequencies;
    iirParameters.channels = channels(p);
    iirParameters.sampleRate = p.sampleRate;
    iirParameters.chunkSize = p.chunkSize;
    initializer->initializeIirFilter(iirParameters);
    filter_ = filterFactory->makeIir();
}

void HearingAidBuilder::prepareFeedbackManagement(const Parameters &p) {
    HearingAidInitializer::FeedbackManagement feedbackManagement;
    feedbackManagement.filterEstimationForgettingFactor =
        p.filterEstimationForgettingFactor;
    feedbackManagement.filterEstimationPowerThreshold =
        p.filterEstimationPowerThreshold;
    feedbackManagement.filterEstimationStepSize = p.filterEstimationStepSize;
    feedbackManagement.signalWhiteningFilterLength =
        p.signalWhiteningFilterLength;
    feedbackManagement.persistentFeedbackFilterLength =
        p.persistentFeedbackFilterLength;
    feedbackManagement.hardwareLatency = p.hardwareLatency;
    feedbackManagement.saveQualityMetric = p.saveQualityMetric;
    if (p.feedback == name(Feedback::on)) {
        feedbackManagement.gain = p.feedbackGain;
        feedbackManagement.adaptiveFilterLength =
            p.adaptiveFeedbackFilterLength;
    } else {
        feedbackManagement.gain = 0;
        feedbackManagement.adaptiveFilterLength = 0;
    }

    initializer->initializeFeedbackManagement(feedbackManagement);
}

void HearingAidBuilder::prepareAutomaticGainControl(const Parameters &p) {
    HearingAidInitializer::AutomaticGainControl automaticGainControl;
    automaticGainControl.crossFrequencies = p.crossFrequencies;
    automaticGainControl.channels = channels(p);
    automaticGainControl.attack = p.attack;
    automaticGainControl.release = p.release;
    automaticGainControl.compressionRatios = p.compressionRatios;
    automaticGainControl.kneepoints = p.kneepoints;
    automaticGainControl.kneepointGains = p.kneepointGains;
    automaticGainControl.broadbandOutputLimitingThresholds =
        p.broadbandOutputLimitingThresholds;
    automaticGainControl.sampleRate = p.sampleRate;
    automaticGainControl.fullScaleLevel = p.fullScaleLevel;
    initializer->initializeAutomaticGainControl(automaticGainControl);
}

std::shared_ptr<Filter> HearingAidBuilder::filter() {
    return filter_;
}
}
