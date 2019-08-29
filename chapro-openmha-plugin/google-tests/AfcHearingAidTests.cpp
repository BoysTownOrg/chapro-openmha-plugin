#include <gsl/gsl>
#include <memory>

namespace hearing_aid {
class SuperSignalProcessor {
public:
    using real_type = float;
    virtual ~SuperSignalProcessor() = default;
    virtual void feedbackCancelInput(int) = 0;
    virtual void compressInput(int) = 0;
    virtual void filterbankAnalyze(int) = 0;
    virtual void compressChannel(int) = 0;
    virtual void filterbankSynthesize(int) = 0;
    virtual void compressOutput(int) = 0;
    virtual void feedbackCancelOutput(int) = 0;
    virtual int chunkSize() = 0;
};

class AfcHearingAid {
    std::shared_ptr<SuperSignalProcessor> processor;
public:
    using signal_type = gsl::span<SuperSignalProcessor::real_type>;
    explicit AfcHearingAid(
        std::shared_ptr<SuperSignalProcessor> processor
    ) : processor{std::move(processor)} {}

    void process(signal_type signal) {
        const auto chunkSize = processor->chunkSize();
        if (signal.size() != chunkSize)
            return;
        processor->feedbackCancelInput(chunkSize);
        processor->compressInput(chunkSize);
        processor->filterbankAnalyze(chunkSize);
        processor->compressChannel(chunkSize);
        processor->filterbankSynthesize(chunkSize);
        processor->compressOutput(chunkSize);
        processor->feedbackCancelOutput(chunkSize);
    }
};
}

#include "LogString.h"
#include "assert-utility.h"
#include <gtest/gtest.h>

namespace hearing_aid::tests { namespace {
class SuperSignalProcessorStub : public SuperSignalProcessor {
    LogString log_;
    int chunkSize_;
    int feedbackCancelInputChunkSize_;
    int compressInputChunkSize_;
    int filterbankAnalyzeChunkSize_;
    int compressChannelChunkSize_;
    int filterbankSynthesizeChunkSize_;
    int compressOutputChunkSize_;
    int feedbackCancelOutputChunkSize_;
public:
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

    void feedbackCancelInput(int c) override {
        feedbackCancelInputChunkSize_ = c;
        log_.insert("feedbackCancelInput");
    }

    void compressInput(int c) override {
        compressInputChunkSize_ = c;
        log_.insert("compressInput");
    }

    void filterbankAnalyze(int c) override {
        filterbankAnalyzeChunkSize_ = c;
        log_.insert("filterbankAnalyze");
    }

    void compressChannel(int c) override {
        compressChannelChunkSize_ = c;
        log_.insert("compressChannel");
    }

    void filterbankSynthesize(int c) override {
        filterbankSynthesizeChunkSize_ = c;
        log_.insert("filterbankSynthesize");
    }

    void compressOutput(int c) override {
        compressOutputChunkSize_ = c;
        log_.insert("compressOutput");
    }

    void feedbackCancelOutput(int c) override {
        feedbackCancelOutputChunkSize_ = c;
        log_.insert("feedbackCancelOutput");
    }

    void setChunkSize(int c) {
        chunkSize_ = c;
    }

    int chunkSize() override {
        return chunkSize_;
    }
};

class AfcHearingAidTests : public ::testing::Test {
    using signal_type = AfcHearingAid::signal_type;
    using buffer_type = std::vector<signal_type::element_type>;
    std::shared_ptr<SuperSignalProcessorStub> superSignalProcessor =
        std::make_shared<SuperSignalProcessorStub>();
    AfcHearingAid hearingAid{superSignalProcessor};
protected:
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

    void process(signal_type x) {
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
}}