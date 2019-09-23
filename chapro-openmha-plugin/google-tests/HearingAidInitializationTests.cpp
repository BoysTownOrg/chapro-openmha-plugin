#include <hearing-aid/HearingAidInitialization.h>
#include <gtest/gtest.h>
#include <string>

namespace hearing_aid { namespace {
void assertTrue(bool c) {
    EXPECT_TRUE(c);
}

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
        assertTrue(initializer_.firInitialized());
    }

    void assertIirInitialized() {
        assertTrue(initializer_.iirInitialized());
    }

    void assertIirNotInitialized() {
        EXPECT_FALSE(initializer_.iirInitialized());
    }

    void assertFirNotInitialized() {
        EXPECT_FALSE(initializer_.firInitialized());
    }
};

TEST_F(HearingAidInitializationTests, firOnlyInitializesFir) {
    setFilterType("FIR");
    initialize();
    assertFirInitialized();
    assertIirNotInitialized();
}

TEST_F(HearingAidInitializationTests, iirOnlyInitializesIir) {
    setFilterType("IIR");
    initialize();
    assertIirInitialized();
    assertFirNotInitialized();
}
}}
