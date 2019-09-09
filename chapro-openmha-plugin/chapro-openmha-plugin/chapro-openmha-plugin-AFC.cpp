#include "mha_plugin.hh"
#include <hearing-aid/AfcHearingAid.h>
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

class Chapro : public hearing_aid::SuperSignalProcessor {
    void *cha_pointer[NPTR]{};
    const int channels_;
    const int chunkSize_;
public:
    explicit Chapro(const Parameters &);
    ~Chapro() noexcept override;
    Chapro(Chapro &&) = delete;
    Chapro &operator=(Chapro &&) = delete;
    Chapro(const Chapro &) = delete;
    Chapro &operator=(const Chapro &) = delete;
    void feedbackCancelInput(real_signal_type, real_signal_type, int) override;
    void compressInput(real_signal_type, real_signal_type, int) override;
    void filterbankAnalyze(real_signal_type, complex_signal_type, int) override;
    void compressChannel(complex_signal_type, complex_signal_type, int) override;
    void filterbankSynthesize(complex_signal_type, real_signal_type, int) override;
    void compressOutput(real_signal_type, real_signal_type, int) override;
    void feedbackCancelOutput(real_signal_type, int) override;
    int chunkSize() override;
    int channels() override;
};

static void copy(const std::vector<double> &source, double *destination) {
    using size_type = std::vector<double>::size_type;
    for (size_type i = 0; i < source.size(); ++i)
        destination[i] = source.at(i);
}

Chapro::Chapro(const Parameters &parameters) :
    channels_{ parameters.channels },
    chunkSize_{ parameters.chunkSize }
{
    CHA_DSL dsl{};
    dsl.attack = parameters.attack_ms;
    dsl.release = parameters.release_ms;
    dsl.nchannel = channels_;
    copy(parameters.crossFrequenciesHz, dsl.cross_freq);
    copy(parameters.compressionRatios, dsl.cr);
    copy(parameters.kneepoints_dBSpl, dsl.tk);
    copy(parameters.kneepointGains_dB, dsl.tkgain);
    copy(parameters.broadbandOutputLimitingThresholds_dBSpl, dsl.bolt);
    CHA_WDRC wdrc;
    wdrc.attack = 1;
    wdrc.release = 50;
    wdrc.fs = parameters.sampleRate;
    wdrc.maxdB = parameters.max_dB_Spl;
    wdrc.tkgain = 0;
    wdrc.tk = 105;
    wdrc.cr = 10;
    wdrc.bolt = 105;
    int zerosCount = 4;
    auto size_ = 2*channels_*zerosCount;
    std::vector<float> zeros(size_);
    std::vector<float> poles(size_);
    std::vector<float> gain(channels_);
    std::vector<int> delay(channels_);
    double ir_delay_ms = 2.5;
    cha_iirfb_design(
        zeros.data(),
        poles.data(),
        gain.data(),
        delay.data(),
        dsl.cross_freq,
        channels_,
        zerosCount,
        parameters.sampleRate,
        ir_delay_ms
    );
    cha_iirfb_prepare(
        cha_pointer,
        zeros.data(),
        poles.data(),
        gain.data(),
        delay.data(),
        channels_,
        zerosCount,
        parameters.sampleRate,
        chunkSize_
    );
    cha_afc_prepare(
        cha_pointer,
        parameters.filterEstimationStepSize,
        parameters.filterEstimationForgettingFactor,
        parameters.filterEstimationPowerThreshold,
        parameters.adaptiveFeedbackFilterLength,
        parameters.signalWhiteningFilterLength,
        parameters.persistentFeedbackFilterLength,
        parameters.hardwareLatency,
        parameters.feedbackGain,
        parameters.saveQualityMetric
    );
    cha_agc_prepare(cha_pointer, &dsl, &wdrc);
}

Chapro::~Chapro() noexcept {
    cha_cleanup(cha_pointer);
}

void Chapro::feedbackCancelInput(
    real_signal_type input,
    real_signal_type output,
    int chunkSize
) {
    cha_afc_input(cha_pointer, input.data(), output.data(), chunkSize);
}

void Chapro::compressInput(
    real_signal_type input,
    real_signal_type output,
    int chunkSize
) {
    cha_agc_input(cha_pointer, input.data(), output.data(), chunkSize);
}

void Chapro::filterbankAnalyze(
    real_signal_type input,
    complex_signal_type output,
    int chunkSize
) {
    cha_iirfb_analyze(cha_pointer, input.data(), output.data(), chunkSize);
}

void Chapro::compressChannel(
    complex_signal_type input,
    complex_signal_type output,
    int chunkSize
) {
    cha_agc_channel(cha_pointer, input.data(), output.data(), chunkSize);
}

void Chapro::filterbankSynthesize(
    complex_signal_type input,
    real_signal_type output,
    int chunkSize
) {
    cha_iirfb_synthesize(cha_pointer, input.data(), output.data(), chunkSize);
}

void Chapro::compressOutput(
    real_signal_type input,
    real_signal_type output,
    int chunkSize
) {
    cha_agc_output(cha_pointer, input.data(), output.data(), chunkSize);
}

void Chapro::feedbackCancelOutput(real_signal_type input, int chunkSize) {
    cha_afc_output(cha_pointer, input.data(), chunkSize);
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
    MHAParser::float_t mu;
    MHAParser::float_t rho;
    MHAParser::float_t eps;
    MHAParser::float_t fbg;
    MHAParser::int_t sqm;
    MHAParser::int_t afl;
    MHAParser::int_t wfl;
    MHAParser::int_t pfl;
    MHAParser::int_t hdel;
    std::unique_ptr<hearing_aid::AfcHearingAid> hearingAid;
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
        mu{"AFC filter-estimation step size", "0", "[,]"},
        rho{"AFC filter-estimation forgetting factory", "0", "[,]"},
        eps{"AFC filter-estimation power threshold", "0", "[,]"},
        fbg{"simulated-feedback gain", "0", "[,]"},
        sqm{"option to save quality metric", "0", "[,]"},
        afl{"length of adaptive-feedback-filter response", "0", "[,]"},
        wfl{"length of signal-whitening-filter response", "0", "[,]"},
        pfl{"length of persistent-feedback-filter response", "0", "[,]"},
        hdel{"output-to-input hardware delay (samples)", "0", "[,]"}
    {
        insert_item("cross_freq", &cross_freq);
        insert_item("cr", &cr);
        insert_item("tk", &tk);
        insert_item("tkgain", &tkgain);
        insert_item("bolt", &bolt);
        insert_item("attack", &attack);
        insert_item("release", &release);
        insert_item("maxdB", &maxdB);
        insert_item("mu", &mu);
        insert_item("rho", &rho);
        insert_item("eps", &eps);
        insert_item("fbg", &fbg);
        insert_item("sqm", &sqm);
        insert_item("afl", &afl);
        insert_item("wfl", &wfl);
        insert_item("pfl", &pfl);
        insert_item("hdel", &hdel);
    }

    mha_wave_t *process(mha_wave_t * signal) {
        hearingAid->process({
            signal->buf,
            gsl::narrow<hearing_aid::AfcHearingAid::signal_type::index_type>(
                signal->num_frames
            )
        });
        return signal;
    }

    void prepare(mhaconfig_t &configuration) override {
        hearing_aid::SuperSignalProcessor::Parameters p;
        p.sampleRate = configuration.srate;
        p.chunkSize = configuration.fragsize;
        p.channels = cross_freq.data.size() + 1;
        p.attack_ms = attack.data;
        p.release_ms = release.data;
        p.max_dB_Spl = maxdB.data;
        p.filterEstimationStepSize = mu.data;
        p.filterEstimationForgettingFactor = rho.data;
        p.filterEstimationPowerThreshold = eps.data;
        p.feedbackGain = fbg.data;
        p.saveQualityMetric = sqm.data;
        p.adaptiveFeedbackFilterLength = afl.data;
        p.signalWhiteningFilterLength = wfl.data;
        p.persistentFeedbackFilterLength = pfl.data;
        p.hardwareLatency = hdel.data;
        p.compressionRatios = {cr.data.begin(), cr.data.end()};
        p.broadbandOutputLimitingThresholds_dBSpl =
            {bolt.data.begin(), bolt.data.end()};
        p.crossFrequenciesHz =
            {cross_freq.data.begin(), cross_freq.data.end()};
        p.kneepointGains_dB =
            {tkgain.data.begin(), tkgain.data.end()};
        p.kneepoints_dBSpl =
            {tk.data.begin(), tk.data.end()};
        hearingAid = std::make_unique<hearing_aid::AfcHearingAid>(
            std::make_unique<Chapro>(p)
        );
    }
};

MHAPLUGIN_CALLBACKS(chapro, ChaproOpenMhaPlugin, wave, wave)
MHAPLUGIN_DOCUMENTATION(chapro, "text", "here")
