#include "LogString.h"
#include "assert-utility.h"
#include <hearing-aid/AfcHearingAid.h>
#include <gtest/gtest.h>

namespace hearing_aid::tests { namespace {
class SuperSignalProcessorStub : public SuperSignalProcessor, public Filter {
    LogString log_;
    complex_signal_type filterbankSynthesizeInput_;
    complex_signal_type compressChannelOutput_;
    complex_signal_type compressChannelInput_;
    complex_signal_type filterbankAnalyzeOutput_;
    real_signal_type feedbackCancelInputInput_;
    real_signal_type feedbackCancelInputOutput_;
    real_signal_type filterbankAnalyzeInput_;
    real_signal_type filterbankSynthesizeOutput_;
    real_signal_type compressInputInput_;
    real_signal_type compressInputOutput_;
    real_signal_type compressOutputInput_;
    real_signal_type compressOutputOutput_;
    real_signal_type feedbackCancelOutputInput_;
    int chunkSize_;
    int feedbackCancelInputChunkSize_;
    int compressInputChunkSize_;
    int filterbankAnalyzeChunkSize_;
    int compressChannelChunkSize_;
    int filterbankSynthesizeChunkSize_;
    int compressOutputChunkSize_;
    int feedbackCancelOutputChunkSize_;
    int channels_;
public:
    auto feedbackCancelInputInput() {
        return feedbackCancelInputInput_;
    }

    auto feedbackCancelInputOutput() {
        return feedbackCancelInputOutput_;
    }

    auto filterbankAnalyzeInput() {
        return filterbankAnalyzeInput_;
    }

    auto filterbankSynthesizeOutput() {
        return filterbankSynthesizeOutput_;
    }

    auto compressInputInput() {
        return compressInputInput_;
    }

    auto compressInputOutput() {
        return compressInputOutput_;
    }

    auto compressOutputInput() {
        return compressOutputInput_;
    }

    auto compressOutputOutput() {
        return compressOutputOutput_;
    }

    auto feedbackCancelOutputInput() {
        return feedbackCancelOutputInput_;
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

    auto &log() const {
        return log_;
    }

    int feedbackCancelInputChunkSize() {
        return feedbackCancelInputChunkSize_;
    }

    int compressInputChunkSize() {
        return compressInputChunkSize_;
    }

    int filterbankAnalyzeChunkSize() {
        return filterbankAnalyzeChunkSize_;
    }

    int compressChannelChunkSize() {
        return compressChannelChunkSize_;
    }

    int filterbankSynthesizeChunkSize() {
        return filterbankSynthesizeChunkSize_;
    }

    int compressOutputChunkSize() {
        return compressOutputChunkSize_;
    }

    int feedbackCancelOutputChunkSize() {
        return feedbackCancelOutputChunkSize_;
    }

    void feedbackCancelInput(
        real_signal_type a,
        real_signal_type b,
        int c
    ) override {
        feedbackCancelInputInput_ = a;
        feedbackCancelInputOutput_ = b;
        feedbackCancelInputChunkSize_ = c;
        log_.insert("feedbackCancelInput");
    }

    void compressInput(real_signal_type a, real_signal_type b, int c) override {
        compressInputInput_ = a;
        compressInputOutput_ = b;
        compressInputChunkSize_ = c;
        log_.insert("compressInput");
    }

    void filterbankAnalyze(
        real_signal_type a,
        complex_signal_type b,
        int c
    ) override {
        filterbankAnalyzeInput_ = a;
        filterbankAnalyzeOutput_ = b;
        filterbankAnalyzeChunkSize_ = c;
        log_.insert("filterbankAnalyze");
    }

    void compressChannel(
        complex_signal_type in,
        complex_signal_type out,
        int c
    ) override {
        compressChannelInput_ = in;
        compressChannelOutput_ = out;
        compressChannelChunkSize_ = c;
        log_.insert("compressChannel");
    }

    void filterbankSynthesize(complex_signal_type in, real_signal_type b, int c) override {
        filterbankSynthesizeInput_ = in;
        filterbankSynthesizeOutput_ = b;
        filterbankSynthesizeChunkSize_ = c;
        log_.insert("filterbankSynthesize");
    }

    void compressOutput(
        real_signal_type a,
        real_signal_type b,
        int c
    ) override {
        compressOutputInput_ = a;
        compressOutputOutput_ = b;
        compressOutputChunkSize_ = c;
        log_.insert("compressOutput");
    }

    void feedbackCancelOutput(real_signal_type a, int c) override {
        feedbackCancelOutputInput_ = a;
        feedbackCancelOutputChunkSize_ = c;
        log_.insert("feedbackCancelOutput");
    }

    void setChunkSize(int c) {
        chunkSize_ = c;
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

class AfcHearingAidTests : public ::testing::Test {
protected:
    using buffer_type = std::vector<real_signal_type::element_type>;
    std::shared_ptr<SuperSignalProcessorStub> superSignalProcessor =
        std::make_shared<SuperSignalProcessorStub>();

    auto &signalProcessingLog() {
        return superSignalProcessor->log();
    }

    void process() {
        buffer_type x(superSignalProcessor->chunkSize());
        process(x);
    }

    void processUnequalChunk() {
        setChunkSize(1);
        buffer_type x(2);
        process(x);
    }

    void process(real_signal_type x) {
        AfcHearingAid hearingAid{superSignalProcessor, superSignalProcessor};
        hearingAid.process(x);
    }

    void assertEachChunkSizeEquals(int c) {
        assertEqual(c, superSignalProcessor->feedbackCancelInputChunkSize());
        assertEqual(c, superSignalProcessor->compressInputChunkSize());
        assertEqual(c, superSignalProcessor->filterbankAnalyzeChunkSize());
        assertEqual(c, superSignalProcessor->compressChannelChunkSize());
        assertEqual(c, superSignalProcessor->filterbankSynthesizeChunkSize());
        assertEqual(c, superSignalProcessor->compressOutputChunkSize());
        assertEqual(c, superSignalProcessor->feedbackCancelOutputChunkSize());
    }

    void setChunkSize(int c) {
        superSignalProcessor->setChunkSize(c);
    }

    void setChannels(int c) {
        superSignalProcessor->setChannels(c);
    }

    void assertEachComplexSize(
        complex_signal_type::size_type c
    ) {
        assertEqual(c, superSignalProcessor->filterbankAnalyzeOutput().size());
        assertEqual(c, superSignalProcessor->compressChannelInput().size());
        assertEqual(c, superSignalProcessor->compressChannelOutput().size());
        assertEqual(c, superSignalProcessor->filterbankSynthesizeInput().size());
    }

    void assertEachComplexBufferEqual() {
        assertEqual(
            superSignalProcessor->compressChannelInput(),
            superSignalProcessor->filterbankAnalyzeOutput()
        );
        assertEqual(
            superSignalProcessor->compressChannelOutput(),
            superSignalProcessor->compressChannelInput()
        );
        assertEqual(
            superSignalProcessor->filterbankSynthesizeInput(),
            superSignalProcessor->compressChannelOutput()
        );
    }

    void assertEachRealBufferEquals(real_signal_type x) {
        assertEqual(x, superSignalProcessor->feedbackCancelInputInput());
        assertEqual(x, superSignalProcessor->feedbackCancelInputOutput());
        assertEqual(x, superSignalProcessor->filterbankAnalyzeInput());
        assertEqual(x, superSignalProcessor->filterbankSynthesizeOutput());
        assertEqual(x, superSignalProcessor->compressInputInput());
        assertEqual(x, superSignalProcessor->compressInputOutput());
        assertEqual(x, superSignalProcessor->compressOutputInput());
        assertEqual(x, superSignalProcessor->compressOutputOutput());
        assertEqual(x, superSignalProcessor->feedbackCancelOutputInput());
    }
};

TEST_F(AfcHearingAidTests, invokesFunctionsInOrder) {
    process();
    assertEqual(
        "feedbackCancelInput"
        "compressInput"
        "filterbankAnalyze"
        "compressChannel"
        "filterbankSynthesize"
        "compressOutput"
        "feedbackCancelOutput",
        signalProcessingLog()
    );
}

TEST_F(
    AfcHearingAidTests,
    processDoesNotInvokeWhenFrameCountDoesNotEqualChunkSize
) {
    processUnequalChunk();
    assertTrue(signalProcessingLog().isEmpty());
}

TEST_F(AfcHearingAidTests, processPassesChunkSize) {
    setChunkSize(1);
    process();
    assertEachChunkSizeEquals(1);
}

TEST_F(
    AfcHearingAidTests,
    intermediateBufferSizeIsTwiceProductOfChannelsAndChunkSize
) {
    setChunkSize(3);
    setChannels(5);
    process();
    assertEachComplexSize(2 * 3 * 5);
}

TEST_F(
    AfcHearingAidTests,
    eachComplexBufferIsEqual
) {
    setChunkSize(3);
    setChannels(5);
    process();
    assertEachComplexBufferEqual();
}

TEST_F(
    AfcHearingAidTests,
    eachRealBufferIsEqual
) {
    setChunkSize(1);
    buffer_type x(1);
    process(x);
    assertEachRealBufferEquals(x);
}
}}
