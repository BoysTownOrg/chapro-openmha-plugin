#include "LogString.h"
#include "assert-utility.h"
#include <hearing-aid/HearingAid.h>
#include <gtest/gtest.h>

namespace hearing_aid::tests { namespace {
class FilterbankCompressorSpy : public FilterbankCompressor {
    LogString log_{};
    complex_signal_type filterbankAnalyzeOutput_;
    complex_signal_type compressChannelInput_;
    complex_signal_type compressChannelOutput_;
    complex_signal_type filterbankSynthesizeInput_;
    real_signal_type filterbankAnalyzeInput_;
    real_signal_type filterbankSynthesizeOutput_;
    real_signal_type compressInputInput_;
    real_signal_type compressInputOutput_;
    real_signal_type compressOutputInput_;
    real_signal_type compressOutputOutput_;
    int chunkSize_ = 1;
    int channels_ = 1;
    int compressInputChunkSize_{};
    int filterbankAnalyzeChunkSize_{};
    int compressChannelsChunkSize_{};
    int filterbankSynthesizeChunkSize_{};
    int compressOutputChunkSize_{};
public:
    auto filterbankAnalyzeInput() const {
        return filterbankAnalyzeInput_;
    }

    auto filterbankSynthesizeOutput() const {
        return filterbankSynthesizeOutput_;
    }

    auto compressInputInput() const {
        return compressInputInput_;
    }

    auto compressInputOutput() const {
        return compressInputOutput_;
    }

    auto compressOutputInput() const {
        return compressOutputInput_;
    }

    auto compressOutputOutput() const {
        return compressOutputOutput_;
    }

    auto filterbankSynthesizeInput() const {
        return filterbankSynthesizeInput_;
    }

    auto compressChannelOutput() const {
        return compressChannelOutput_;
    }

    auto compressChannelInput() const {
        return compressChannelInput_;
    }

    auto filterbankAnalyzeOutput() const {
        return filterbankAnalyzeOutput_;
    }

    auto &log() const noexcept {
        return log_;
    }

    void compressInput(
        real_signal_type a,
        real_signal_type b,
        int chunkSize
    ) override {
        compressInputInput_ = a;
        compressInputOutput_ = b;
        compressInputChunkSize_ = chunkSize;
        log_.insert("compressInput");
    }

    void analyzeFilterbank(
        real_signal_type a,
        complex_signal_type s,
        int chunkSize
    ) override {
        filterbankAnalyzeInput_ = a;
        filterbankAnalyzeOutput_ = s;
        filterbankAnalyzeChunkSize_ = chunkSize;
        log_.insert("analyzeFilterbank");
    }

    void compressChannels(
        complex_signal_type a,
        complex_signal_type b,
        int chunkSize
    ) override {
        compressChannelInput_ = a;
        compressChannelOutput_ = b;
        compressChannelsChunkSize_ = chunkSize;
        log_.insert("compressChannels");
    }

    void synthesizeFilterbank(
        complex_signal_type a,
        real_signal_type b,
        int chunkSize
    ) override {
        filterbankSynthesizeInput_ = a;
        filterbankSynthesizeOutput_ = b;
        filterbankSynthesizeChunkSize_ = chunkSize;
        log_.insert("synthesizeFilterbank");
    }

    void compressOutput(
        real_signal_type a,
        real_signal_type b,
        int chunkSize
    ) override {
        compressOutputInput_ = a;
        compressOutputOutput_ = b;
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

    void setChannels(int c) {
        channels_ = c;
    }

    int channels() override {
        return channels_;
    }
};

class HearingAidTests : public ::testing::Test {
protected:
    using signal_type = HearingAid::signal_type;
    using buffer_type = std::vector<signal_type::element_type>;
    std::shared_ptr<FilterbankCompressorSpy> compressor =
        std::make_shared<FilterbankCompressorSpy>();

    void processUnequalChunk() {
        buffer_type x(compressor->chunkSize() + 1);
        process(x);
    }

    void process() {
        buffer_type x(compressor->chunkSize());
        process(x);
    }

    void process(signal_type x) {
        HearingAid hearingAid_{ compressor };
        process(hearingAid_, x);
    }

    void process(HearingAid &hearingAid, signal_type x) {
        hearingAid.process(x);
    }

    auto &compressorLog() {
        return compressor->log();
    }

    void setChannels(int c) {
        compressor->setChannels(c);
    }

    void setChunkSize(int c) {
        compressor->setChunkSize(c);
    }

    void assertEachComplexSize(
        FilterbankCompressor::complex_signal_type::size_type c
    ) {
        assertEqual(c, compressor->filterbankAnalyzeOutput().size());
        assertEqual(c, compressor->compressChannelInput().size());
        assertEqual(c, compressor->compressChannelOutput().size());
        assertEqual(c, compressor->filterbankSynthesizeInput().size());
    }

    void assertEachComplexBufferEqual() {
        assertEqual(
            compressor->compressChannelInput(),
            compressor->filterbankAnalyzeOutput()
        );
        assertEqual(
            compressor->compressChannelOutput(),
            compressor->compressChannelInput()
        );
        assertEqual(
            compressor->filterbankSynthesizeInput(),
            compressor->compressChannelOutput()
        );
    }

    void assertEachRealBufferEquals(signal_type x) {
        assertEqual(
            x,
            compressor->filterbankAnalyzeInput()
        );
        assertEqual(
            x,
            compressor->filterbankSynthesizeOutput()
        );
        assertEqual(
            x,
            compressor->compressInputInput()
        );
        assertEqual(
            x,
            compressor->compressInputOutput()
        );
        assertEqual(
            x,
            compressor->compressOutputInput()
        );
        assertEqual(
            x,
            compressor->compressOutputOutput()
        );
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

TEST_F(
    HearingAidTests,
    intermediateBufferSizeIsTwiceProductOfChannelsAndChunkSize
) {
    setChunkSize(3);
    setChannels(5);
    process();
    assertEachComplexSize(2 * 3 * 5);
}

TEST_F(
    HearingAidTests,
    eachComplexBufferIsEqual
) {
    setChunkSize(3);
    setChannels(5);
    process();
    assertEachComplexBufferEqual();
}

TEST_F(
    HearingAidTests,
    eachRealBufferIsEqual
) {
    setChunkSize(1);
    buffer_type x(1);
    process(x);
    assertEachRealBufferEquals(x);
}
}}
