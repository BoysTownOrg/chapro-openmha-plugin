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
        if (signal.size() != compressor->chunkSize())
            return;
        const auto chunkSize = compressor->chunkSize();
        const auto buffer_ = &buffer.front();
        compressor->compressInput(signal.data(), signal.data(), chunkSize);
        compressor->analyzeFilterbank(signal.data(), buffer_, chunkSize);
        compressor->compressChannels(buffer_, buffer_, chunkSize);
        compressor->synthesizeFilterbank(buffer_, signal.data(), chunkSize);
        compressor->compressOutput(signal.data(), signal.data(), chunkSize);
    }
}
