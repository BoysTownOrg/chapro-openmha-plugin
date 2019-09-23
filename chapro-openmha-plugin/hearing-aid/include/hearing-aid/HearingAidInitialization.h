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
    virtual void initializeIirFilter() = 0;
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

class HearingAidInitialization {
    HearingAidInitializer *initializer;
public:
    HearingAidInitialization(HearingAidInitializer *initializer) :
        initializer{initializer} {}

    struct Parameters {
        std::vector<double> crossFrequencies;
        std::string filterType;
        double sampleRate;
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
        else
            initializer->initializeIirFilter();
    }
};
}

#endif
