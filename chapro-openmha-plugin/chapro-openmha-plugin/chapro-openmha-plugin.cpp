#include "mha_plugin.hh"
#include <hearing-aid/HearingAid.h>
#include <hearing-aid/HearingAidInitialization.h>
extern "C" {
#include <chapro.h>
}
#include <gsl/gsl>

// These are defined in chapro.h but appear in some standard headers
#undef _size
#undef fmin
#undef fmove
#undef fcopy
#undef fzero
#undef dcopy
#undef dzero
#undef round
#undef log2

class ChaproInitializer : public hearing_aid::HearingAidInitializer {
    CHA_PTR cha_pointer;
public:
    ChaproInitializer(CHA_PTR cha_pointer) : cha_pointer{cha_pointer} {}

    void initializeFirFilter(const FirParameters &p) override {
        const auto hamming = 0;
        auto mutableCrossFrequencies = p.crossFrequencies;
        cha_firfb_prepare(
            cha_pointer,
            mutableCrossFrequencies.data(),
            p.channels,
            p.sampleRate,
            p.windowSize,
            hamming,
            p.chunkSize
        );
    }

    void initializeIirFilter(const IirParameters &p) override {
        int zerosCount = 4;
        auto size_ = 2*p.channels*zerosCount;
        std::vector<float> zeros(size_);
        std::vector<float> poles(size_);
        std::vector<float> gain(p.channels);
        std::vector<int> delay(p.channels);
        double ir_delay_ms = 2.5;
        auto mutableCrossFrequencies = p.crossFrequencies;
        cha_iirfb_design(
            zeros.data(),
            poles.data(),
            gain.data(),
            delay.data(),
            mutableCrossFrequencies.data(),
            p.channels,
            zerosCount,
            p.sampleRate,
            ir_delay_ms
        );
        cha_iirfb_prepare(
            cha_pointer,
            zeros.data(),
            poles.data(),
            gain.data(),
            delay.data(),
            p.channels,
            zerosCount,
            p.sampleRate,
            p.chunkSize
        );
    }
};

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
    void compressInput(real_signal_type, real_signal_type, int chunkSize) override;
    void analyzeFilterbank(real_signal_type, complex_signal_type output, int chunkSize) override;
    void compressChannels(complex_signal_type input, complex_signal_type output, int chunkSize) override;
    void synthesizeFilterbank(complex_signal_type input, real_signal_type, int chunkSize) override;
    void compressOutput(real_signal_type, real_signal_type, int chunkSize) override;
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

void Chapro::compressInput(real_signal_type input, real_signal_type output, int chunkSize) {
    cha_agc_input(cha_pointer, input.data(), output.data(), chunkSize);
}

void Chapro::analyzeFilterbank(real_signal_type input, complex_signal_type output, int chunkSize) {
    cha_firfb_analyze(cha_pointer, input.data(), output.data(), chunkSize);
}

void Chapro::compressChannels(complex_signal_type input, complex_signal_type output, int chunkSize) {
    cha_agc_channel(cha_pointer, input.data(), output.data(), chunkSize);
}

void Chapro::synthesizeFilterbank(complex_signal_type input, real_signal_type output, int chunkSize) {
    cha_firfb_synthesize(cha_pointer, input.data(), output.data(), chunkSize);
}

void Chapro::compressOutput(real_signal_type input, real_signal_type output, int chunkSize) {
    cha_agc_output(cha_pointer, input.data(), output.data(), chunkSize);
}

int Chapro::chunkSize() {
    return chunkSize_;
}

int Chapro::channels() {
    return channels_;
}

class ChaproOpenMhaPlugin : public MHAPlugin::plugin_t<int> {
    MHAParser::vfloat_t cross_freq;
    MHAParser::vfloat_t cr;
    MHAParser::vfloat_t tk;
    MHAParser::vfloat_t tkgain;
    MHAParser::vfloat_t bolt;
    MHAParser::float_t attack;
    MHAParser::float_t release;
    MHAParser::float_t maxdB;
    MHAParser::int_t nw;
    std::unique_ptr<hearing_aid::HearingAid> hearingAid;
public:
    ChaproOpenMhaPlugin(
        algo_comm_t &ac,
        const std::string &,
        const std::string &
    ) :
        MHAPlugin::plugin_t<int>{{}, ac},
        cross_freq{"cross frequencies (Hz)", "[0]", "[,]"},
        cr{"compression ratio", "[0]", "[,]"},
        tk{"compression-start kneepoint", "[0]", "[,]"},
        tkgain{"compression-start gain", "[0]", "[,]"},
        bolt{"broadband output limiting threshold", "[0]", "[,]"},
        attack{"attack time (ms)", "0", "[,]"},
        release{"release time (ms)", "0", "[,]"},
        maxdB{"maximum output (dB SPL)", "0", "[,]"},
        nw{"window size (samples)", "0", "[,]"}
    {
        set_node_id("chapro");
        insert_item("cross_freq", &cross_freq);
        insert_item("cr", &cr);
        insert_item("tk", &tk);
        insert_item("tkgain", &tkgain);
        insert_item("bolt", &bolt);
        insert_item("attack", &attack);
        insert_item("release", &release);
        insert_item("maxdB", &maxdB);
        insert_item("nw", &nw);
    }
    
    mha_wave_t *process(mha_wave_t * signal) {
        hearingAid->process({
            signal->buf, 
            gsl::narrow<hearing_aid::HearingAid::signal_type::index_type>(signal->num_frames)
        });
        return signal;
    }
    
    void prepare(mhaconfig_t &configuration) override {
        hearing_aid::FilterbankCompressor::Parameters p;
        p.sampleRate = configuration.srate;
        p.chunkSize = configuration.fragsize;
        p.channels = cross_freq.data.size() + 1;
        p.attack_ms = attack.data;
        p.release_ms = release.data;
        p.max_dB_Spl = maxdB.data;
        p.windowSize = nw.data;
        p.compressionRatios = {cr.data.begin(), cr.data.end()};
        p.broadbandOutputLimitingThresholds_dBSpl =
            {bolt.data.begin(), bolt.data.end()};
        p.crossFrequenciesHz =
            {cross_freq.data.begin(), cross_freq.data.end()};
        p.kneepointGains_dB =
            {tkgain.data.begin(), tkgain.data.end()};
        p.kneepoints_dBSpl =
            {tk.data.begin(), tk.data.end()};
        hearingAid = std::make_unique<hearing_aid::HearingAid>(
            std::make_unique<Chapro>(std::move(p))
        );
    }
};

MHAPLUGIN_CALLBACKS(chapro, ChaproOpenMhaPlugin, wave, wave)
MHAPLUGIN_DOCUMENTATION(chapro, "text", "here")
