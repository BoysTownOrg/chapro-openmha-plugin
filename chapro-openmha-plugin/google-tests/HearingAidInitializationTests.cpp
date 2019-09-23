#include <hearing-aid/HearingAidInitialization.h>
#include <gtest/gtest.h>
#include <string>

namespace hearing_aid { namespace {
class HearingAidInitializerStub : public HearingAidInitializer {
    bool firInitialized_{};
    bool iirInitialized_{};
public:
    auto firInitialized() const {
        return firInitialized_;
    }

    auto iirInitialized() const {
        return iirInitialized_;
    }

    void initializeFirFilter() override {
        firInitialized_ = true;
    }

    void initializeIirFilter() override {
        iirInitialized_ = true;
    }
};

class HearingAidInitializationTests : public ::testing::Test {
    HearingAidInitializerStub initializer_;
    HearingAidInitialization initializer{&initializer_};
    HearingAidInitialization::Parameters p;
protected:
    void setFilterType(std::string s) {
        p.filterType = std::move(s);
    }

    void initialize() {
        initializer.initialize(p);
    }

    void assertFirInitialized() {
        EXPECT_TRUE(initializer_.firInitialized());
    }

    void assertIirInitialized() {
        EXPECT_TRUE(initializer_.iirInitialized());
    }
};

TEST_F(HearingAidInitializationTests, firInitializesFir) {
    setFilterType("FIR");
    initialize();
    assertFirInitialized();
}

TEST_F(HearingAidInitializationTests, iirInitializesIir) {
    setFilterType("IIR");
    initialize();
    assertIirInitialized();
}
}}
