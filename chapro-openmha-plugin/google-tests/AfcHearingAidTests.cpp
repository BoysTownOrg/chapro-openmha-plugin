#include <gsl/gsl>
#include <memory>

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
};

class AfcHearingAid {
    std::shared_ptr<SuperSignalProcessor> processor;
public:
    using signal_type = gsl::span<SuperSignalProcessor::real_type>;
    explicit AfcHearingAid(
        std::shared_ptr<SuperSignalProcessor> processor
    ) : processor{std::move(processor)} {}

    void process(signal_type) {
        processor->feedbackCancelInput();
        processor->compressInput();
        processor->filterbankAnalyze();
        processor->compressChannel();
        processor->filterbankSynthesize();
        processor->compressOutput();
        processor->feedbackCancelOutput();
    }
};

#include "LogString.h"
#include "assert-utility.h"
#include <gtest/gtest.h>

namespace hearing_aid::tests { namespace {
class SuperSignalProcessorStub : public SuperSignalProcessor {
    LogString log_;
public:
    auto &log() const {
        return log_;
    }

    void feedbackCancelInput() {
        log_.insert("feedbackCancelInput");
    }
    void compressInput() {
        log_.insert("compressInput");
    }
    void filterbankAnalyze() {
        log_.insert("filterbankAnalyze");
    }
    void compressChannel() {
        log_.insert("compressChannel");
    }
    void filterbankSynthesize() {
        log_.insert("filterbankSynthesize");
    }
    void compressOutput() {
        log_.insert("compressOutput");
    }
    void feedbackCancelOutput() {
        log_.insert("feedbackCancelOutput");
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
};

TEST_F(AfcHearingAidTests, tbd) {
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
}}