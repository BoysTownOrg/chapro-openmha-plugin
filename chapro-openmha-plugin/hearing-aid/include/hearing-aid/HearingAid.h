#ifndef hearing_aid_HearingAid_h
#define hearing_aid_HearingAid_h

#include <gsl/gsl>
#include <vector>
#include <memory>

namespace hearing_aid {
class FilterbankCompressor {
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
        int windowSize;
        int chunkSize;
        int channels;
    };
    virtual ~FilterbankCompressor() = default;
    using real_type = float;
    using complex_type = float;
    using complex_signal_type = gsl::span<complex_type>;
    virtual void compressInput(
        real_type *input,
        real_type *output,
        int chunkSize
    ) = 0;
    virtual void analyzeFilterbank(
        real_type *input,
        complex_type *output,
        complex_signal_type,
        int chunkSize
    ) = 0;
    virtual void compressChannels(
        complex_type *input,
        complex_type *output,
        complex_signal_type,
        complex_signal_type,
        int chunkSize
    ) = 0;
    virtual void synthesizeFilterbank(
        complex_signal_type,
        complex_type *input,
        real_type *output,
        int chunkSize
    ) = 0;
    virtual void compressOutput(
        real_type *input,
        real_type *output,
        int chunkSize
    ) = 0;
    virtual int chunkSize() = 0;
    virtual int channels() = 0;
};

class HearingAid {
    // Order important for construction.
    std::vector<FilterbankCompressor::complex_type> buffer;
    std::shared_ptr<FilterbankCompressor> compressor;
public:
    using signal_type = gsl::span<FilterbankCompressor::real_type>;
    explicit HearingAid(
        std::shared_ptr<FilterbankCompressor>
    );
    void process(signal_type);
};
}

#endif
