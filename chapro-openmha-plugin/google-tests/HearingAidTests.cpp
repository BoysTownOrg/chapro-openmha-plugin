#include <hearing-aid/HearingAid.h>
#include <gtest/gtest.h>
#include <sstream>

namespace {
    template<typename T>
    void assertEqual(const T &expected, const T &actual) {
        EXPECT_EQ(expected, actual);
    }
    
    void assertEqual(const std::string &expected, const std::string &actual) {
        EXPECT_EQ(expected, actual);
    }
    
    void assertTrue(bool c) {
        EXPECT_TRUE(c);
    }
    
    class LogString {
        std::stringstream s{};
    public:
        void insert(std::string s_) {
            s << std::move(s_);
        }

        bool isEmpty() const {
            return s.str().empty();
        }

        bool beginsWith(std::string const &beginning) const {
            if (s.str().length() >= beginning.length())
                return 0 == s.str().compare(0, beginning.length(), beginning);
            else
                return false;
        }

        bool endsWith(std::string const &ending) const {
            if (s.str().length() >= ending.length())
                return 0 == s.str().compare(
                    s.str().length() - ending.length(),
                    ending.length(),
                    ending);
            else
                return false;
        }

        bool contains(const std::string &s2) const {
            return s.str().find(s2) != std::string::npos;
        }
        
        operator std::string() const { return s.str(); }
    };

    class FilterbankCompressorSpy : public hearing_aid::FilterbankCompressor {
        LogString log_{};
        int chunkSize_ = 1;
        int compressInputChunkSize_{};
        int filterbankAnalyzeChunkSize_{};
        int compressChannelsChunkSize_{};
        int filterbankSynthesizeChunkSize_{};
        int compressOutputChunkSize_{};
        int windowSize_{};
        bool failed_{};
    public:
        auto &log() const noexcept {
            return log_;
        }

        void compressInput(real_type *, real_type *, int chunkSize) override {
            compressInputChunkSize_ = chunkSize;
            log_.insert("compressInput");
        }

        void analyzeFilterbank(real_type *, complex_type *, int chunkSize) override {
            filterbankAnalyzeChunkSize_ = chunkSize;
            log_.insert("analyzeFilterbank");
        }

        void compressChannels(complex_type *, complex_type *, int chunkSize) override {
            compressChannelsChunkSize_ = chunkSize;
            log_.insert("compressChannels");
        }

        void synthesizeFilterbank(complex_type *, real_type *, int chunkSize) override {
            filterbankSynthesizeChunkSize_ = chunkSize;
            log_.insert("synthesizeFilterbank");
        }

        void compressOutput(real_type *, real_type *, int chunkSize) override {
            compressOutputChunkSize_ = chunkSize;
            log_.insert("compressOutput");
        }

        void setChunkSize(int s) noexcept {
            chunkSize_ = s;
        }

        auto compressInputChunkSize() const noexcept {
            return compressInputChunkSize_;
        }

        auto filterbankAnalyzeChunkSize() const noexcept {
            return filterbankAnalyzeChunkSize_;
        }

        auto compressChannelsChunkSize() const noexcept {
            return compressChannelsChunkSize_;
        }

        auto filterbankSynthesizeChunkSize() const noexcept {
            return filterbankSynthesizeChunkSize_;
        }

        auto compressOutputChunkSize() const noexcept {
            return compressOutputChunkSize_;
        }

        int chunkSize() override {
            return chunkSize_;
        }

        void fail() noexcept {
            failed_ = true;
        }

        bool failed() override {
            return failed_;
        }

        void setWindowSize(int n) noexcept {
            windowSize_ = n;
        }

        int windowSize() override {
            return windowSize_;
        }

        int channels() override {
            return 1;
        }
    };

    std::shared_ptr<FilterbankCompressorSpy> compressorWithValidDefaults() {
        auto c = std::make_shared<FilterbankCompressorSpy>();
        c->setChunkSize(1);
        c->setWindowSize(1);
        return c;
    }
    
    class HearingAidTests : public ::testing::Test {
    protected:
        using signal_type = hearing_aid::HearingAid::signal_type;
        using buffer_type = std::vector<signal_type::element_type>;
        std::shared_ptr<FilterbankCompressorSpy> compressor =
            compressorWithValidDefaults();
        hearing_aid::HearingAid hearingAid{ compressor };

        void processUnequalChunk() {
            buffer_type x(compressor->chunkSize() + 1);
            process(x);
        }
        
        void process() {
            buffer_type x(compressor->chunkSize());
            process(x);
        }
        
        void process(signal_type x) {
            hearingAid.process(x);
        }
        
        auto &compressorLog() {
            return compressor->log();
        }
    };
    
    TEST_F(
        HearingAidTests,
        processCallsCompressorMethodsInCorrectOrder
    ) {
        process();
        assertEqual(
            "compressInput"
            "analyzeFilterbank"
            "compressChannels"
            "synthesizeFilterbank"
            "compressOutput",
            compressorLog()
        );
    }
    
    TEST_F(
        HearingAidTests,
        processDoesNotInvokeCompressorWhenFrameCountDoesNotEqualChunkSize
    ) {
        processUnequalChunk();
        assertTrue(compressorLog().isEmpty());
    }

    TEST_F(HearingAidTests, processPassesChunkSize) {
        compressor->setChunkSize(1);
        process();
        assertEqual(1, compressor->compressInputChunkSize());
        assertEqual(1, compressor->filterbankAnalyzeChunkSize());
        assertEqual(1, compressor->compressChannelsChunkSize());
        assertEqual(1, compressor->filterbankSynthesizeChunkSize());
        assertEqual(1, compressor->compressOutputChunkSize());
    }
}
