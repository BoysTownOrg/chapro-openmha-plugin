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

/** This C++ class implements the simplest example plugin for the
 * step-by-step tutorial.  It inherits from MHAPlugin::plugin_t for
 * correct integration in the configuration language interface.  */
class example1_t : public MHAPlugin::plugin_t<int> {
public:
  /** Do-nothing constructor.  The constructor has to take these three
   * arguments, but it does not have to use them. However, the base
   * class has to be initialized. */
  example1_t(algo_comm_t & ac,
            const std::string &,
            const std::string &)
      : MHAPlugin::plugin_t<int>("",ac)
  {/* Do nothing in constructor */}

  /** Release may be empty */
  void release(void)
  {/* Do nothing in release */}

  /** Plugin preparation. This plugin checks that the input signal has the
   * waveform domain and contains at least one channel
   * @param signal_info
   *   Structure containing a description of the form of the signal (domain,
   *   number of channels, frames per block, sampling rate.
   */
  void prepare(mhaconfig_t & signal_info)
  {
    if (signal_info.domain != MHA_WAVEFORM)
      throw MHA_Error(__FILE__, __LINE__,
                      "This plugin can only process waveform signals.");
    if (signal_info.channels < 1)
      throw MHA_Error(__FILE__,__LINE__,
                      "This plugin requires at least one input channel.");
  }

  /** Signal processing performed by the plugin.
   * This plugin multiplies the signal in the first audio channel by
   * a factor 0.1.
   * @param signal
   *   Pointer to the input signal structure.
   * @return
   *   Returns a pointer to the input signal structure,
   *   with a the signal modified by this plugin.
   *   (In-place processing)
   */
  mha_wave_t * process(mha_wave_t * signal)
  {
    unsigned int channel = 0; // channels and frames counting starts with 0
    float factor = 0.1f;
    unsigned int frame;

    // Scale channel number "channel" by "factor":
    for(frame = 0; frame < signal->num_frames; frame++) {
      // Waveform channels are stored interleaved.
      signal->buf[signal->num_channels * frame + channel] *= factor;
    }
    // Algorithms may process data in-place and return the input signal
    // structure as their output signal:
    return signal;
  }
};

/*
 * This macro connects the example1_t class with the \mha plugin C interface
 * The first argument is the class name, the other arguments define the
 * input and output domain of the algorithm.
 */
MHAPLUGIN_CALLBACKS(example1,example1_t,wave,wave)

/*
 * This macro creates code classification of the plugin and for
 * automatic documentation.
 *
 * The first argument to the macro is a space separated list of
 * categories, starting with the most relevant category. The second
 * argument is a LaTeX-compatible character array with some detailed
 * documentation of the plugin.
 */
MHAPLUGIN_DOCUMENTATION\
(example1,
 "example level-modification audio-channels",
 "The {\\bf simplest} example of an \\mha{} plugin.\n\n"
 "This plugin scales one channel of the input signal, working in the "
 "time domain."
 )

// Local Variables:
// compile-command: "make"
// c-basic-offset: 4
// indent-tabs-mode: nil
// coding: utf-8-unix
// End:
