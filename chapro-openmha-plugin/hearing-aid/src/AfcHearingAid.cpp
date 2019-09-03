#include "AfcHearingAid.h"

namespace hearing_aid {
AfcHearingAid::AfcHearingAid(
    std::shared_ptr<SuperSignalProcessor> processor
) :
    buffer(2 * processor->chunkSize() * processor->channels()),
    processor{std::move(processor)} {}

void AfcHearingAid::process(signal_type signal)  {
    const auto chunkSize = processor->chunkSize();
    if (signal.size() != chunkSize)
        return;
    auto signal_ = signal.data();
    processor->feedbackCancelInput(signal_, signal_, chunkSize);
    processor->compressInput(signal_, signal_, chunkSize);
    processor->filterbankAnalyze(signal_, buffer, chunkSize);
    processor->compressChannel(buffer, buffer, chunkSize);
    processor->filterbankSynthesize(buffer, signal_, chunkSize);
    processor->compressOutput(signal_, signal_, chunkSize);
    processor->feedbackCancelOutput(signal_, chunkSize);
}
}
