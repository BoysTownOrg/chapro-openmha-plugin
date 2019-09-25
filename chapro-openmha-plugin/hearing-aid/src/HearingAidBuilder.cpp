#include "HearingAidBuilder.h"

namespace hearing_aid {
void HearingAidBuilder::build(const Parameters &p) {
    if (p.filterType == name(FilterType::fir)) {
        HearingAidInitializer::FirParameters firParameters;
        firParameters.crossFrequencies = p.crossFrequencies;
        firParameters.channels = channels(p);
        firParameters.sampleRate = p.sampleRate;
        firParameters.windowSize = p.windowSize;
        firParameters.chunkSize = p.chunkSize;
        initializer->initializeFirFilter(firParameters);
    } else {
        HearingAidInitializer::IirParameters iirParameters;
        iirParameters.crossFrequencies = p.crossFrequencies;
        iirParameters.channels = channels(p);
        iirParameters.sampleRate = p.sampleRate;
        iirParameters.chunkSize = p.chunkSize;
        initializer->initializeIirFilter(iirParameters);
    }
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

int HearingAidBuilder::channels(const Parameters &p) {
    return p.crossFrequencies.size() + 1;
}

std::shared_ptr<Filter> HearingAidBuilder::iirFilter() {
    return filterFactory->makeIir();
}
}
