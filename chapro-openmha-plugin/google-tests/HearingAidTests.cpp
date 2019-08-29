#include "LogString.h"
#include "assert-utility.h"
#include <hearing-aid/HearingAid.h>
#include <gtest/gtest.h>

namespace hearing_aid::tests { namespace {
class FilterbankCompressorSpy : public FilterbankCompressor {
    LogString log_{};
    int chunkSize_ = 1;
    int compressInputChunkSize_{};
    int filterbankAnalyzeChunkSize_{};
    int compressChannelsChunkSize_{};
    int filterbankSynthesizeChunkSize_{};
    int compressOutputChunkSize_{};
public:
    auto &log() const noexcept {
        return log_;
    }

    void compressInput(
        real_type *,
        real_type *,
        int chunkSize
    ) override {
        compressInputChunkSize_ = chunkSize;
        log_.insert("compressInput");
    }

    void analyzeFilterbank(
        real_type *,
        complex_type *,
        int chunkSize
    ) override {
        filterbankAnalyzeChunkSize_ = chunkSize;
        log_.insert("analyzeFilterbank");
    }

    void compressChannels(
        complex_type *,
        complex_type *,
        int chunkSize
    ) override {
        compressChannelsChunkSize_ = chunkSize;
        log_.insert("compressChannels");
    }

    void synthesizeFilterbank(
        complex_type *,
        real_type *,
        int chunkSize
    ) override {
        filterbankSynthesizeChunkSize_ = chunkSize;
        log_.insert("synthesizeFilterbank");
    }

    void compressOutput(
        real_type *,
        real_type *,
        int chunkSize
    ) override {
        compressOutputChunkSize_ = chunkSize;
        log_.insert("compressOutput");
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

    int channels() override {
        return 1;
    }
};

class HearingAidTests : public ::testing::Test {
protected:
    using signal_type = HearingAid::signal_type;
    using buffer_type = std::vector<signal_type::element_type>;
    std::shared_ptr<FilterbankCompressorSpy> compressor =
        std::make_shared<FilterbankCompressorSpy>();
    HearingAid hearingAid{ compressor };

    void processUnequalChunk() {
        buffer_type x(compressor->chunkSize() + 1);
        process(x);
    }
    
    void process() {
        buffer_type x(compressor->chunkSize());
        process(x);
    }
    
    void process(signal_type x) {
        process(hearingAid, x);
    }
    
    void process(HearingAid &hearingAid, signal_type x) {
        hearingAid.process(x);
    }
    
    auto &compressorLog() {
        return compressor->log();
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
        compressorLog()
    );
}

TEST_F(
    HearingAidTests,
    processDoesNotInvokeCompressorWhenFrameCountDoesNotEqualChunkSize
) {
    processUnequalChunk();
    assertTrue(compressorLog().isEmpty());
}

TEST_F(HearingAidTests, processPassesChunkSize) {
    compressor->setChunkSize(1);
    process();
    assertEqual(1, compressor->compressInputChunkSize());
    assertEqual(1, compressor->filterbankAnalyzeChunkSize());
    assertEqual(1, compressor->compressChannelsChunkSize());
    assertEqual(1, compressor->filterbankSynthesizeChunkSize());
    assertEqual(1, compressor->compressOutputChunkSize());
}

class MultipliesRealSignalsByPrimes : public FilterbankCompressor {
public:
    void compressInput(
        real_type *input,
        real_type *output,
        int
    ) override {
        *input *= 2;
        *output *= 3;
    }

    void analyzeFilterbank(
        real_type *input,
        complex_type *,
        int
    ) override {
        *input *= 5;
    }

    void synthesizeFilterbank(
        complex_type *,
        real_type *output,
        int
    ) override {
        *output *= 7;
    }

    void compressOutput(
        real_type *input,
        real_type *output,
        int
    ) override {
        *input *= 11;
        *output *= 13;
    }

    int chunkSize() override { return 1; }
    int channels() override { return 1; }
    void compressChannels(complex_type *, complex_type *, int) override {}
};

TEST_F(
    HearingAidTests,
    processPassesRealInputsAppropriately
) {
    HearingAid hearingAid{
        std::make_shared<MultipliesRealSignalsByPrimes>()
    };
    buffer_type x = { 4 };
    process(hearingAid, x);
    assertEqual({ 4 * 2 * 3 * 5 * 7 * 11 * 13 }, x);
}

class ForComplexSignalTests : public FilterbankCompressor {
    complex_type postSynthesizeFilterbankComplexResult_{};
    int chunkSize_{ 1 };
    int channels_{ 1 };
    int pointerOffset_{};
public:
    void analyzeFilterbank(
        real_type *,
        complex_type *output,
        int
    ) override {
        *(output + pointerOffset_) += 7;
        *(output + pointerOffset_) *= 11;
    }

    void compressChannels(
        complex_type *input,
        complex_type *output,
        int
    ) override {
        *(input + pointerOffset_) *= 13;
        *(output + pointerOffset_) *= 17;
    }

    void synthesizeFilterbank(
        complex_type *input,
        real_type *,
        int
    ) override {
        *(input + pointerOffset_) *= 19;
        postSynthesizeFilterbankComplexResult_ = *(input + pointerOffset_);
    }

    int chunkSize() override {
        return chunkSize_;
    }

    int channels() override {
        return channels_;
    }

    void setChunkSize(int s) {
        chunkSize_ = s;
    }

    void setChannels(int c) {
        channels_ = c;
    }

    void setPointerOffset(int offset) {
        pointerOffset_ = offset;
    }

    complex_type postSynthesizeFilterbankComplexResult() const {
        return postSynthesizeFilterbankComplexResult_;
    }

    void compressInput(real_type *, real_type *, int) override {}
    void compressOutput(real_type *, real_type *, int) override {}
};

TEST_F(
    HearingAidTests,
    processPassesComplexInputsAppropriately
) {
    auto compressor = std::make_shared<ForComplexSignalTests>();
    HearingAid hearingAid{compressor};
    buffer_type x(1);
    process(hearingAid, x);
    assertEqual(
        (0 + 7) * 11 * 13 * 17 * 19.0f,
        compressor->postSynthesizeFilterbankComplexResult()
    );
}

TEST_F(
    HearingAidTests,
    complexInputSizeIsAtLeastChannelTimesChunkSizeTimesTwo
) {
    auto compressor = std::make_shared<ForComplexSignalTests>();
    compressor->setChunkSize(4);
    compressor->setChannels(5);
    HearingAid hearingAid{compressor};
    compressor->setPointerOffset(4 * 5 * 2 - 1);
    buffer_type x(4);
    process(hearingAid, x);
    assertEqual(
        (0 + 7) * 11 * 13 * 17 * 19.0f,
        compressor->postSynthesizeFilterbankComplexResult()
    );
}
}}
