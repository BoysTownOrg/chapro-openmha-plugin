#include <gsl/gsl>
#include <memory>

namespace hearing_aid {
class SuperSignalProcessor {
public:
    using real_type = float;
    using complex_type = float;
    virtual ~SuperSignalProcessor() = default;
    virtual void feedbackCancelInput(real_type *, real_type *, int) = 0;
    virtual void compressInput(real_type *, real_type *, int) = 0;
    virtual void filterbankAnalyze(real_type *, complex_type *, int) = 0;
    virtual void compressChannel(real_type *, real_type *, int) = 0;
    virtual void filterbankSynthesize(complex_type *, real_type *, int) = 0;
    virtual void compressOutput(real_type *, real_type *, int) = 0;
    virtual void feedbackCancelOutput(real_type *, int) = 0;
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
        auto signal_ = signal.data();
        processor->feedbackCancelInput(signal_, signal_, chunkSize);
        processor->compressInput(signal_, signal_, chunkSize);
        processor->filterbankAnalyze(signal_, {}, chunkSize);
        processor->compressChannel(signal_, signal_, chunkSize);
        processor->filterbankSynthesize({}, signal_, chunkSize);
        processor->compressOutput(signal_, signal_, chunkSize);
        processor->feedbackCancelOutput(signal_, chunkSize);
    }
};
}

#include "LogString.h"
#include "assert-utility.h"
#include <gtest/gtest.h>

namespace hearing_aid::tests { namespace {
class SettableChunkSizeSuperSignalProcessor : public virtual SuperSignalProcessor {
public:
    virtual void setChunkSize(int) = 0;
};

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

    void feedbackCancelInput(real_type *, real_type *, int c) override {
        feedbackCancelInputChunkSize_ = c;
        log_.insert("feedbackCancelInput");
    }

    void compressInput(real_type *, real_type *, int c) override {
        compressInputChunkSize_ = c;
        log_.insert("compressInput");
    }

    void filterbankAnalyze(real_type *, complex_type *, int c) override {
        filterbankAnalyzeChunkSize_ = c;
        log_.insert("filterbankAnalyze");
    }

    void compressChannel(real_type *, real_type *, int c) override {
        compressChannelChunkSize_ = c;
        log_.insert("compressChannel");
    }

    void filterbankSynthesize(complex_type *, real_type *, int c) override {
        filterbankSynthesizeChunkSize_ = c;
        log_.insert("filterbankSynthesize");
    }

    void compressOutput(real_type *, real_type *, int c) override {
        compressOutputChunkSize_ = c;
        log_.insert("compressOutput");
    }

    void feedbackCancelOutput(real_type *, int c) override {
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

    void assertTransformation(
        std::shared_ptr<SettableChunkSizeSuperSignalProcessor> p,
        buffer_type x,
        buffer_type y
    ) {
        p->setChunkSize(x.size());
        AfcHearingAid hearingAid_{std::move(p)};
        hearingAid_.process(x);
        assertEqual(y, x);
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

class MultipliesRealSignalsByPrimes : public SettableChunkSizeSuperSignalProcessor {
    int chunkSize_;
public:
    void compressInput(
        real_type *input,
        real_type *output,
        int
    ) override {
        *input *= 2;
        *output *= 3;
    }

    void filterbankAnalyze(
        real_type *input,
        complex_type *,
        int
    ) override {
        *input *= 5;
    }

    void filterbankSynthesize(
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

    void feedbackCancelInput(
        real_type *input,
        real_type *output,
        int
    ) override {
        *input *= 17;
        *output *= 19;
    }

    void feedbackCancelOutput(
        real_type *input,
        int
    ) override {
        *input *= 23;
    }

    void setChunkSize(int c) override {
        chunkSize_ = c;
    }

    int chunkSize() override { return chunkSize_; }

    void compressChannel(real_type *, real_type *, int) override {}
};

TEST_F(
    AfcHearingAidTests,
    processPassesRealInputsAppropriately
) {
    assertTransformation(
        std::make_shared<MultipliesRealSignalsByPrimes>(),
        { 0.5 },
        { 0.5 * 2 * 3 * 5 * 7 * 11 * 13 * 17 * 19 * 23 }
    );
}
}}