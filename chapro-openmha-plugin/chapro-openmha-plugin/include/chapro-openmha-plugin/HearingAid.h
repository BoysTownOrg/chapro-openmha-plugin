#ifndef chapro_openmha_plugin_HearingAid_h
#define chapro_openmha_plugin_HearingAid_h

#include <vector>

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
    virtual void compressInput(
        real_type *input,
        real_type *output,
        int chunkSize
    ) = 0;
    virtual void analyzeFilterbank(
        real_type *input,
        complex_type *output,
        int chunkSize
    ) = 0;
    virtual void compressChannels(
        complex_type *input,
        complex_type *output,
        int chunkSize
    ) = 0;
    virtual void synthesizeFilterbank(
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
    virtual bool failed() = 0;
    virtual int channels() = 0;
    virtual int windowSize() = 0;
};

#include <memory>
#include <gsl/gsl>

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
private:
    void throwIfNotPowerOfTwo(int n, std::string name);
};

HearingAid::HearingAid(
    std::shared_ptr<FilterbankCompressor> compressor
) :
    buffer(compressor->channels() * compressor->chunkSize() * 2),
    compressor{ std::move(compressor) }
{
}

void HearingAid::process(signal_type signal) {
    if (signal.size() != compressor->chunkSize())
        return;
    const auto chunkSize = compressor->chunkSize();
    const auto buffer_ = &buffer.front();
    compressor->compressInput(signal.data(), signal.data(), chunkSize);
    compressor->analyzeFilterbank(signal.data(), buffer_, chunkSize);
    compressor->compressChannels(buffer_, buffer_, chunkSize);
    compressor->synthesizeFilterbank(buffer_, signal.data(), chunkSize);
    compressor->compressOutput(signal.data(), signal.data(), chunkSize);
}

#endif
