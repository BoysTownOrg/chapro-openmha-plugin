#include "HearingAidInitialization.h"

namespace hearing_aid {
void HearingAidInitialization::initialize(const Parameters &p) {
    if (p.filterType == name(FilterType::fir)) {
        HearingAidInitializer::FirParameters firParameters;
        firParameters.crossFrequencies = p.crossFrequencies;
        firParameters.channels = p.crossFrequencies.size() + 1;
        firParameters.sampleRate = p.sampleRate;
        firParameters.windowSize = p.windowSize;
        firParameters.chunkSize = p.chunkSize;
        initializer->initializeFirFilter(firParameters);
    } else {
        HearingAidInitializer::IirParameters iirParameters;
        iirParameters.crossFrequencies = p.crossFrequencies;
        iirParameters.channels = p.crossFrequencies.size() + 1;
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
    }
    else {
        feedbackManagement.gain = 0;
        feedbackManagement.adaptiveFilterLength = 0;
    }

    initializer->initializeFeedbackManagement(feedbackManagement);
    HearingAidInitializer::AutomaticGainControl automaticGainControl;
    automaticGainControl.crossFrequencies = p.crossFrequencies;
    automaticGainControl.channels = p.crossFrequencies.size()+1;
    automaticGainControl.attack = p.attack;
    automaticGainControl.release = p.release;
    initializer->initializeAutomaticGainControl(automaticGainControl);
}
}