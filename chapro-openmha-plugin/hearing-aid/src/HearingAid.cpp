#include "HearingAid.h"

namespace hearing_aid {
HearingAid::HearingAid(
    std::shared_ptr<FilterbankCompressor> compressor
) :
    buffer(compressor->channels() * compressor->chunkSize() * 2),
    compressor{ std::move(compressor) }
{
}

void HearingAid::process(signal_type signal) {
    const auto chunkSize = compressor->chunkSize();
    if (signal.size() != chunkSize)
        return;
    auto signal_ = signal.data();
    compressor->compressInput(signal_, signal_, chunkSize);
    compressor->analyzeFilterbank(signal_, buffer, chunkSize);
    compressor->compressChannels(buffer, buffer, chunkSize);
    compressor->synthesizeFilterbank(buffer, signal_, chunkSize);
    compressor->compressOutput(signal_, signal_, chunkSize);
}
}
