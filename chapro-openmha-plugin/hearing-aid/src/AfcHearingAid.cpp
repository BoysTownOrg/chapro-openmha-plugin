#include "AfcHearingAid.h"

namespace hearing_aid {
AfcHearingAid::AfcHearingAid(
    std::shared_ptr<SuperSignalProcessor> processor,
    std::shared_ptr<Filter> filter
) :
    buffer(2 * processor->chunkSize() * processor->channels()),
    processor{std::move(processor)},
    filter{std::move(filter)} {}

void AfcHearingAid::process(real_signal_type signal)  {
    const auto chunkSize = processor->chunkSize();
    if (signal.size() != chunkSize)
        return;
    processor->feedbackCancelInput(signal, signal, chunkSize);
    processor->compressInput(signal, signal, chunkSize);
    filter->filterbankAnalyze(signal, buffer, chunkSize);
    processor->compressChannel(buffer, buffer, chunkSize);
    filter->filterbankSynthesize(buffer, signal, chunkSize);
    processor->compressOutput(signal, signal, chunkSize);
    processor->feedbackCancelOutput(signal, chunkSize);
}
}
