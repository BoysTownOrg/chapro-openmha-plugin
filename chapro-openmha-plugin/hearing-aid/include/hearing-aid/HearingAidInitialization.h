#ifndef CHAPRO_OPENMHA_PLUGIN_HEARING_AID_INCLUDE_HEARING_AID_HEARINGAIDINITIALIZATION_H_
#define CHAPRO_OPENMHA_PLUGIN_HEARING_AID_INCLUDE_HEARING_AID_HEARINGAIDINITIALIZATION_H_

#include <string>

namespace hearing_aid {
class HearingAidInitializer {
public:
    virtual ~HearingAidInitializer() = default;
    virtual void initializeFirFilter() = 0;
    virtual void initializeIirFilter() = 0;
};

class HearingAidInitialization {
    HearingAidInitializer *initializer;
public:
    HearingAidInitialization(HearingAidInitializer *initializer) :
        initializer{initializer} {}

    struct Parameters {
        std::string filterType;
    };

    void initialize(const Parameters &) {
        initializer->initializeFirFilter();
        initializer->initializeIirFilter();
    }
};
}

#endif
