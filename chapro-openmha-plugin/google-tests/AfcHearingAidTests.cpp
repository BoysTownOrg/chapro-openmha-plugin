#include <gsl/gsl>
#include <memory>

namespace hearing_aid {
class SuperSignalProcessor {
public:
    using real_type = float;
    virtual ~SuperSignalProcessor() = default;
    virtual void feedbackCancelInput() = 0;
    virtual void compressInput() = 0;
    virtual void filterbankAnalyze() = 0;
    virtual void compressChannel() = 0;
    virtual void filterbankSynthesize() = 0;
    virtual void compressOutput() = 0;
    virtual void feedbackCancelOutput() = 0;
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
        processor->feedbackCancelInput();
        processor->compressInput();
        processor->filterbankAnalyze();
        processor->compressChannel();
        processor->filterbankSynthesize();
        processor->compressOutput();
        processor->feedbackCancelOutput();
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
public:
    auto &log() const {
        return log_;
    }

    void feedbackCancelInput() override {
        log_.insert("feedbackCancelInput");
    }

    void compressInput() override {
        log_.insert("compressInput");
    }

    void filterbankAnalyze() override {
        log_.insert("filterbankAnalyze");
    }

    void compressChannel() override {
        log_.insert("compressChannel");
    }

    void filterbankSynthesize() override {
        log_.insert("filterbankSynthesize");
    }

    void compressOutput() override {
        log_.insert("compressOutput");
    }

    void feedbackCancelOutput() override {
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
    std::shared_ptr<SuperSignalProcessorStub> superSignalProcessor = 
        std::make_shared<SuperSignalProcessorStub>();
    AfcHearingAid hearingAid{superSignalProcessor};
protected:
    auto &signalProcessingLog() {
        return superSignalProcessor->log();
    }

    void process() {
        hearingAid.process({});
    }

    void processUnequalChunk() {
        superSignalProcessor->setChunkSize(1);
        std::vector<float> x(2);
        hearingAid.process(x);
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
}}