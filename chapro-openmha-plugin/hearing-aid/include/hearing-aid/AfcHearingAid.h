#ifndef AFC_HEARING_AID_H_
#define AFC_HEARING_AID_H_

#include <gsl/gsl>
#include <memory>
#include <vector>

namespace hearing_aid {
class SuperSignalProcessor {
public:
    struct Parameters {
        std::vector<double> crossFrequenciesHz;
        std::vector<double> compressionRatios;
        std::vector<double> kneepointGains_dB;
        std::vector<double> kneepoints_dBSpl;
        std::vector<double> broadbandOutputLimitingThresholds_dBSpl;
        double attack_ms;
        double release_ms;
        double sampleRate;
        double max_dB_Spl;
        double filterEstimationStepSize;
        double filterEstimationForgettingFactor;
        double filterEstimationPowerThreshold;
        double feedbackGain;
        int saveQualityMetric;
        int adaptiveFeedbackFilterLength;
        int signalWhiteningFilterLength;
        int persistentFeedbackFilterLength;
        int hardwareLatency;
        int chunkSize;
        int channels;
    };
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
    explicit AfcHearingAid(std::shared_ptr<SuperSignalProcessor>);
    void process(signal_type signal);
};
}

#endif
