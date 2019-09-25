#ifndef CHAPRO_OPENMHA_PLUGIN_HEARING_AID_INCLUDE_HEARING_AID_HEARINGAIDBUILDER_H_
#define CHAPRO_OPENMHA_PLUGIN_HEARING_AID_INCLUDE_HEARING_AID_HEARINGAIDBUILDER_H_

#include "AfcHearingAid.h"
#include <string>
#include <vector>
#include <memory>

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
        int persistentFeedbackFilterLength;
        int hardwareLatency;
        int saveQualityMetric;
    };
    virtual void initializeFeedbackManagement(const FeedbackManagement &) = 0;
    struct AutomaticGainControl {
        std::vector<double> crossFrequencies;
        std::vector<double> compressionRatios;
        std::vector<double> kneepoints;
        std::vector<double> kneepointGains;
        std::vector<double> broadbandOutputLimitingThresholds;
        double attack;
        double release;
        double sampleRate;
        double fullScaleLevel;
        int channels;
    };
    virtual void initializeAutomaticGainControl(
        const AutomaticGainControl &
    ) = 0;
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
        default:
            return "";
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
        default:
            return "";
    }
}

class HearingAidBuilder {
    std::shared_ptr<Filter> filter_;
    HearingAidInitializer *initializer;
    FilterFactory *filterFactory;
public:
    HearingAidBuilder(
        HearingAidInitializer *initializer,
        FilterFactory *filterFactory
    ) :
        initializer{initializer},
        filterFactory{filterFactory} {}

    struct Parameters {
        std::vector<double> crossFrequencies;
        std::vector<double> compressionRatios;
        std::vector<double> kneepoints;
        std::vector<double> kneepointGains;
        std::vector<double> broadbandOutputLimitingThresholds;
        std::string filterType;
        std::string feedback;
        double attack;
        double release;
        double sampleRate;
        double fullScaleLevel;
        double feedbackGain;
        double filterEstimationForgettingFactor;
        double filterEstimationPowerThreshold;
        double filterEstimationStepSize;
        int adaptiveFeedbackFilterLength;
        int signalWhiteningFilterLength;
        int persistentFeedbackFilterLength;
        int hardwareLatency;
        int saveQualityMetric;
        int windowSize;
        int chunkSize;
    };

    void build(const Parameters &);
    std::shared_ptr<Filter> filter();
private:
    void prepareFilter(const Parameters &);
    void buildFirFilter(const Parameters &);
    void buildIirFilter(const Parameters &);
    void prepareFeedbackManagement(const Parameters &);
    void prepareAutomaticGainControl(const Parameters &);
    int channels(const Parameters &);
};
}

#endif
