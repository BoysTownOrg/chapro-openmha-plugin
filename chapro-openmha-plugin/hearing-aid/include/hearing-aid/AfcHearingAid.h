#ifndef CHAPRO_OPENMHA_PLUGIN_HEARING_AID_INCLUDE_HEARING_AID_AFCHEARINGAID_H_
#define CHAPRO_OPENMHA_PLUGIN_HEARING_AID_INCLUDE_HEARING_AID_AFCHEARINGAID_H_

#include <gsl/gsl>
#include <memory>
#include <vector>

namespace hearing_aid {
using real_type = float;
using complex_type = float;
using complex_signal_type = gsl::span<complex_type>;
using real_signal_type = gsl::span<real_type>;
class Filter {
public:
    virtual ~Filter() = default;
    virtual void filterbankAnalyze(
        real_signal_type,
        complex_signal_type,
        int
    ) = 0;
    virtual void filterbankSynthesize(
        complex_signal_type,
        real_signal_type,
        int
    ) = 0;
};

class FilterFactory {
public:
    virtual ~FilterFactory() = default;
    virtual std::shared_ptr<Filter> makeIir() = 0;
    virtual std::shared_ptr<Filter> makeFir() = 0;
};

class SuperSignalProcessor {
public:
    struct Parameters {
        int chunkSize;
        int channels;
    };
    virtual ~SuperSignalProcessor() = default;
    virtual void feedbackCancelInput(
        real_signal_type,
        real_signal_type,
        int
    ) = 0;
    virtual void compressInput(real_signal_type, real_signal_type, int) = 0;
    virtual void compressChannel(
        complex_signal_type,
        complex_signal_type,
        int
    ) = 0;
    virtual void compressOutput(real_signal_type, real_signal_type, int) = 0;
    virtual void feedbackCancelOutput(real_signal_type, int) = 0;
    virtual int chunkSize() = 0;
    virtual int channels() = 0;
};

class AfcHearingAid {
    std::vector<complex_type> buffer;
    std::shared_ptr<SuperSignalProcessor> processor;
    std::shared_ptr<Filter> filter;
public:
    using signal_type = gsl::span<real_type>;
    AfcHearingAid(
        std::shared_ptr<SuperSignalProcessor>,
        std::shared_ptr<Filter>
    );
    void process(signal_type signal);
};
}

#endif
