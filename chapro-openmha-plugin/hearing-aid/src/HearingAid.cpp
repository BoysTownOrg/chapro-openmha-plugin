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
    const auto buffer_ = &buffer.front();
    auto signal_ = signal.data();
    compressor->compressInput(signal_, signal_, chunkSize);
    compressor->analyzeFilterbank(signal_, buffer_, chunkSize);
    compressor->compressChannels(buffer_, buffer_, chunkSize);
    compressor->synthesizeFilterbank(buffer_, signal_, chunkSize);
    compressor->compressOutput(signal_, signal_, chunkSize);
}
}
