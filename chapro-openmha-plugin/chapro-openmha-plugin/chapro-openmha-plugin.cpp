#include "mha_plugin.hh"
#include <hearing-aid/HearingAid.h>
extern "C" {
#include <chapro.h>
#include <cha_ff.h>
}

// These are (unfortunately) defined in chapro.h but appear in some standard headers
#undef _size
#undef fmin
#undef fmove
#undef fcopy
#undef fzero
#undef dcopy
#undef dzero
#undef round
#undef log2

class Chapro : public hearing_aid::FilterbankCompressor {
    void *cha_pointer[NPTR]{};
    const int channels_;
    const int chunkSize_;
    const int windowSize_;
    int error = 0;
public:
    explicit Chapro(Parameters);
    ~Chapro() noexcept override;
    Chapro(Chapro &&) = delete;
    Chapro &operator=(Chapro &&) = delete;
    Chapro(const Chapro &) = delete;
    Chapro &operator=(const Chapro &) = delete;
    void compressInput(real_type * input, real_type * output, int chunkSize) override;
    void analyzeFilterbank(real_type * input, complex_type * output, int chunkSize) override;
    void compressChannels(complex_type * input, complex_type * output, int chunkSize) override;
    void synthesizeFilterbank(complex_type * input, real_type * output, int chunkSize) override;
    void compressOutput(real_type * input, real_type * output, int chunkSize) override;
    int chunkSize() override;
    int channels() override;
};

Chapro::Chapro(Parameters parameters) :
    channels_{ parameters.channels },
    chunkSize_{ parameters.chunkSize },
    windowSize_{ parameters.windowSize }
{
    CHA_DSL dsl{};
    dsl.attack = parameters.attack_ms;
    dsl.release = parameters.release_ms;
    dsl.nchannel = channels_;
    using size_type = std::vector<double>::size_type;
    for (size_type i = 0; i < parameters.crossFrequenciesHz.size(); ++i)
        dsl.cross_freq[i] = parameters.crossFrequenciesHz.at(i);
    for (size_type i = 0; i < parameters.compressionRatios.size(); ++i)
        dsl.cr[i] = parameters.compressionRatios.at(i);
    for (size_type i = 0; i < parameters.kneepoints_dBSpl.size(); ++i)
        dsl.tk[i] = parameters.kneepoints_dBSpl.at(i);
    for (size_type i = 0; i < parameters.kneepointGains_dB.size(); ++i)
        dsl.tkgain[i] = parameters.kneepointGains_dB.at(i);
    for (
        size_type i = 0;
        i < parameters.broadbandOutputLimitingThresholds_dBSpl.size();
        ++i
    )
        dsl.bolt[i] = parameters.broadbandOutputLimitingThresholds_dBSpl.at(i);
    CHA_WDRC wdrc;
    wdrc.attack = 1;
    wdrc.release = 50;
    wdrc.fs = parameters.sampleRate;
    wdrc.maxdB = parameters.max_dB_Spl;
    wdrc.tkgain = 0;
    wdrc.tk = 105;
    wdrc.cr = 10;
    wdrc.bolt = 105;
    const auto hamming = 0;
    error = cha_firfb_prepare(
        cha_pointer,
        dsl.cross_freq,
        channels_,
        parameters.sampleRate,
        windowSize_,
        hamming,
        chunkSize_
    );
    error |= cha_agc_prepare(cha_pointer, &dsl, &wdrc);
}

Chapro::~Chapro() noexcept {
    cha_cleanup(cha_pointer);
}

void Chapro::compressInput(float *input, float *output, int chunkSize) {
    cha_agc_input(cha_pointer, input, output, chunkSize);
}

void Chapro::analyzeFilterbank(float *input, float *output, int chunkSize) {
    cha_firfb_analyze(cha_pointer, input, output, chunkSize);
}

void Chapro::compressChannels(float *input, float *output, int chunkSize) {
    cha_agc_channel(cha_pointer, input, output, chunkSize);
}

void Chapro::synthesizeFilterbank(float *input, float *output, int chunkSize) {
    cha_firfb_synthesize(cha_pointer, input, output, chunkSize);
}

void Chapro::compressOutput(float *input, float *output, int chunkSize) {
    cha_agc_output(cha_pointer, input, output, chunkSize);
}

int Chapro::chunkSize() {
    return chunkSize_;
}

int Chapro::channels() {
    return channels_;
}

std::shared_ptr<hearing_aid::FilterbankCompressor> compressor() {
    hearing_aid::FilterbankCompressor::Parameters p;
    p.channels = 8;
    p.attack_ms = 5;
    p.release_ms = 50;
    p.chunkSize = 64;
    p.max_dB_Spl = 119;
    p.sampleRate = 44100;
    p.windowSize = 256;
    p.compressionRatios = { 1.1, 1.1, 1.2, 1.1, 1.5, 1.6, 1.6, 2.1 };
    p.broadbandOutputLimitingThresholds_dBSpl =
        { 100, 100, 100, 100, 100, 100, 100, 100 };
    p.crossFrequenciesHz =
        { 317, 503, 798, 1265, 2006, 3181, 5045 };
    p.kneepointGains_dB =
        { 2, 2, 10, 2, 27, 36, 34, 49 };
    p.kneepoints_dBSpl =
        { 32, 32, 37, 29, 38, 39, 35, 39 };
    return std::make_shared<Chapro>(std::move(p));
}

class ChaproOpenMhaPlugin : public MHAPlugin::plugin_t<int> {
    hearing_aid::HearingAid hearingAid;
public:
    ChaproOpenMhaPlugin(
        algo_comm_t &ac,
        const std::string &,
        const std::string &
    ) :
        MHAPlugin::plugin_t<int>{{}, ac},
        hearingAid{compressor()} {}
    
    mha_wave_t *process(mha_wave_t * signal) {
        hearingAid.process({signal->buf, signal->num_frames});
        return signal;
    }
    
    void prepare(mhaconfig_t &) override {}
};

MHAPLUGIN_CALLBACKS(chapro, ChaproOpenMhaPlugin, wave, wave)
MHAPLUGIN_DOCUMENTATION(chapro, "text", "here")
