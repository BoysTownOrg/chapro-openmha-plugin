#ifndef AFC_HEARING_AID_H_
#define AFC_HEARING_AID_H_

#include <gsl/gsl>
#include <memory>
#include <vector>

namespace hearing_aid {
class SuperSignalProcessor {
public:
    using real_type = float;
    using complex_type = float;
    using complex_signal_type = gsl::span<complex_type>;
    virtual ~SuperSignalProcessor() = default;
    virtual void feedbackCancelInput(real_type *, real_type *, int) = 0;
    virtual void compressInput(real_type *, real_type *, int) = 0;
    virtual void filterbankAnalyze(real_type *, complex_signal_type, int) = 0;
    virtual void compressChannel(complex_signal_type, complex_signal_type, int) = 0;
    virtual void filterbankSynthesize(complex_signal_type, real_type *, int) = 0;
    virtual void compressOutput(real_type *, real_type *, int) = 0;
    virtual void feedbackCancelOutput(real_type *, int) = 0;
    virtual int chunkSize() = 0;
    virtual int channels() = 0;
};

class AfcHearingAid {
    std::vector<SuperSignalProcessor::complex_type> buffer;
    std::shared_ptr<SuperSignalProcessor> processor;
public:
    using signal_type = gsl::span<SuperSignalProcessor::real_type>;
    explicit AfcHearingAid(
        std::shared_ptr<SuperSignalProcessor> processor
    );

    void process(signal_type signal);
};
}

#endif
