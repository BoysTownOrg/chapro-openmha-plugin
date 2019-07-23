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

#include <gtest/gtest.h>
#include <sstream>

class LogString {
    std::stringstream s{};
public:
    void insert(std::string s_) {
        s << std::move(s_);
    }

    bool isEmpty() const {
        return s.str().empty();
    }

    bool beginsWith(std::string const &beginning) const {
        if (s.str().length() >= beginning.length())
            return 0 == s.str().compare(0, beginning.length(), beginning);
        else
            return false;
    }

    bool endsWith(std::string const &ending) const {
        if (s.str().length() >= ending.length())
            return 0 == s.str().compare(
                s.str().length() - ending.length(),
                ending.length(),
                ending);
        else
            return false;
    }

    bool contains(std::string s2) const {
        return s.str().find(std::move(s2)) != std::string::npos;
    }
    
    operator std::string() const { return s.str(); }
};

class FilterbankCompressorSpy : public FilterbankCompressor {
    LogString processingLog_{};
    int chunkSize_ = 1;
    int compressInputChunkSize_{};
    int filterbankAnalyzeChunkSize_{};
    int compressChannelsChunkSize_{};
    int filterbankSynthesizeChunkSize_{};
    int compressOutputChunkSize_{};
    int windowSize_{};
    bool failed_{};
public:
    auto &log() const noexcept {
        return processingLog_;
    }

    void compressInput(real_type *, real_type *, int chunkSize) override {
        compressInputChunkSize_ = chunkSize;
        processingLog_.insert("compressInput");
    }

    void analyzeFilterbank(real_type *, complex_type *, int chunkSize) override {
        filterbankAnalyzeChunkSize_ = chunkSize;
        processingLog_.insert("analyzeFilterbank");
    }

    void compressChannels(complex_type *, complex_type *, int chunkSize) override {
        compressChannelsChunkSize_ = chunkSize;
        processingLog_.insert("compressChannels");
    }

    void synthesizeFilterbank(complex_type *, real_type *, int chunkSize) override {
        filterbankSynthesizeChunkSize_ = chunkSize;
        processingLog_.insert("synthesizeFilterbank");
    }

    void compressOutput(real_type *, real_type *, int chunkSize) override {
        compressOutputChunkSize_ = chunkSize;
        processingLog_.insert("compressOutput");
    }

    void setChunkSize(int s) noexcept {
        chunkSize_ = s;
    }

    auto compressInputChunkSize() const noexcept {
        return compressInputChunkSize_;
    }

    auto filterbankAnalyzeChunkSize() const noexcept {
        return filterbankAnalyzeChunkSize_;
    }

    auto compressChannelsChunkSize() const noexcept {
        return compressChannelsChunkSize_;
    }

    auto filterbankSynthesizeChunkSize() const noexcept {
        return filterbankSynthesizeChunkSize_;
    }

    auto compressOutputChunkSize() const noexcept {
        return compressOutputChunkSize_;
    }

    int chunkSize() override {
        return chunkSize_;
    }

    void fail() noexcept {
        failed_ = true;
    }

    bool failed() override {
        return failed_;
    }

    void setWindowSize(int n) noexcept {
        windowSize_ = n;
    }

    int windowSize() override {
        return windowSize_;
    }

    int channels() override {
        return 1;
    }
};

namespace {
    std::shared_ptr<FilterbankCompressorSpy> compressorWithValidDefaults() {
        auto c = std::make_shared<FilterbankCompressorSpy>();
        c->setChunkSize(1);
        c->setWindowSize(1);
        return c;
    }
    
    void assertEqual(std::string expected, std::string actual) {
        EXPECT_EQ(expected, actual);
    }
    
    class HearingAidTests : public ::testing::Test {
    protected:
        using signal_type = HearingAid::signal_type;
        using buffer_type = std::vector<signal_type::element_type>;
        std::shared_ptr<FilterbankCompressorSpy> compressor =
            compressorWithValidDefaults();
        HearingAid hearingAid{ compressor };

        void process() {
            buffer_type x(compressor->chunkSize());
            hearingAid.process(x);
        }
    };
    
    TEST_F(
        HearingAidTests,
        processCallsCompressorMethodsInCorrectOrder
    ) {
        process();
        assertEqual(
            "compressInput"
            "analyzeFilterbank"
            "compressChannels"
            "synthesizeFilterbank"
            "compressOutput",
            compressor->log()
        );
    }
}
