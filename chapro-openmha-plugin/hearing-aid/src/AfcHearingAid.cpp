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
    processor->feedbackCancelInput(signal, signal, chunkSize);
    processor->compressInput(signal, signal, chunkSize);
    processor->filterbankAnalyze(signal, buffer, chunkSize);
    processor->compressChannel(buffer, buffer, chunkSize);
    processor->filterbankSynthesize(buffer, signal, chunkSize);
    processor->compressOutput(signal, signal, chunkSize);
    processor->feedbackCancelOutput(signal, chunkSize);
}
}
