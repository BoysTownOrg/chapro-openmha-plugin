#ifndef CHAPRO_OPENMHA_PLUGIN_HEARING_AID_INCLUDE_HEARING_AID_HEARINGAIDINITIALIZATION_H_
#define CHAPRO_OPENMHA_PLUGIN_HEARING_AID_INCLUDE_HEARING_AID_HEARINGAIDINITIALIZATION_H_

#include <string>
#include <vector>

namespace hearing_aid {
class HearingAidInitializer {
public:
    virtual ~HearingAidInitializer() = default;
    struct FirParameters {
        std::vector<double> crossFrequencies;
        double sampleRate;
        int channels;
        int windowSize;
        int chunkSize;
    };
    virtual void initializeFirFilter(const FirParameters &) = 0;
    struct IirParameters {
        std::vector<double> crossFrequencies;
        double sampleRate;
        int channels;
        int chunkSize;
    };
    virtual void initializeIirFilter(const IirParameters &) = 0;
    struct FeedbackManagement {
        double gain;
        double filterEstimationForgettingFactor;
        double filterEstimationPowerThreshold;
        double filterEstimationStepSize;
        int adaptiveFilterLength;
        int signalWhiteningFilterLength;
    };
    virtual void initializeFeedbackManagement(const FeedbackManagement &) = 0;
};

enum class FilterType {
    fir,
    iir
};

constexpr const char *name(FilterType t) {
    switch (t) {
        case FilterType::fir:
            return "FIR";
        case FilterType::iir:
            return "IIR";
    }
}

enum class Feedback {
    on,
    off
};

constexpr const char *name(Feedback t) {
    switch (t) {
        case Feedback::on:
            return "yes";
        case Feedback::off:
            return "off";
    }
}

class HearingAidInitialization {
    HearingAidInitializer *initializer;
public:
    explicit HearingAidInitialization(HearingAidInitializer *initializer) :
        initializer{initializer} {}

    struct Parameters {
        std::vector<double> crossFrequencies;
        std::string filterType;
        std::string feedback;
        double sampleRate;
        double feedbackGain;
        double filterEstimationForgettingFactor;
        double filterEstimationPowerThreshold;
        double filterEstimationStepSize;
        int adaptiveFeedbackFilterLength;
        int signalWhiteningFilterLength;
        int windowSize;
        int chunkSize;
    };

    void initialize(const Parameters &p) {
        if (p.filterType == name(FilterType::fir)) {
            HearingAidInitializer::FirParameters firParameters;
            firParameters.crossFrequencies = p.crossFrequencies;
            firParameters.channels = p.crossFrequencies.size() + 1;
            firParameters.sampleRate = p.sampleRate;
            firParameters.windowSize = p.windowSize;
            firParameters.chunkSize = p.chunkSize;
            initializer->initializeFirFilter(firParameters);
        }
        else {
            HearingAidInitializer::IirParameters iirParameters;
            iirParameters.crossFrequencies = p.crossFrequencies;
            iirParameters.channels = p.crossFrequencies.size() + 1;
            iirParameters.sampleRate = p.sampleRate;
            iirParameters.chunkSize = p.chunkSize;
            initializer->initializeIirFilter(iirParameters);
        }
        HearingAidInitializer::FeedbackManagement feedbackManagement;
        feedbackManagement.filterEstimationForgettingFactor = p.filterEstimationForgettingFactor;
        feedbackManagement.filterEstimationPowerThreshold = p.filterEstimationPowerThreshold;
        feedbackManagement.filterEstimationStepSize = p.filterEstimationStepSize;
        feedbackManagement.signalWhiteningFilterLength = p.signalWhiteningFilterLength;
        if (p.feedback == name(Feedback::on)) {
            feedbackManagement.gain = p.feedbackGain;
            feedbackManagement.adaptiveFilterLength = p.adaptiveFeedbackFilterLength;
        }
        else {
            feedbackManagement.gain = 0;
            feedbackManagement.adaptiveFilterLength = 0;
        }

        initializer->initializeFeedbackManagement(feedbackManagement);
    }
};
}

#endif
