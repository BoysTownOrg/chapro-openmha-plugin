#include <hearing-aid/HearingAidInitialization.h>
#include <gtest/gtest.h>
#include <string>

namespace hearing_aid { namespace {
void assertTrue(bool c) {
    EXPECT_TRUE(c);
}

void assertFalse(bool c) {
    EXPECT_FALSE(c);
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
    void setFilterType(FilterType t) {
        setFilterType(name(t));
    }

    void setFilterType(std::string s) {
        p.filterType = std::move(s);
    }

    void initialize() {
        initializer.initialize(p);
    }

    bool firInitialized() {
        return initializer_.firInitialized();
    }

    bool iirInitialized() {
        return initializer_.iirInitialized();
    }

    void assertFirInitialized() {
        assertTrue(firInitialized());
    }

    void assertIirInitialized() {
        assertTrue(iirInitialized());
    }

    void assertIirNotInitialized() {
        assertFalse(iirInitialized());
    }

    void assertFirNotInitialized() {
        assertFalse(firInitialized());
    }
};

TEST_F(HearingAidInitializationTests, firOnlyInitializesFir) {
    setFilterType(FilterType::fir);
    initialize();
    assertFirInitialized();
    assertIirNotInitialized();
}

TEST_F(HearingAidInitializationTests, iirOnlyInitializesIir) {
    setFilterType(FilterType::iir);
    initialize();
    assertIirInitialized();
    assertFirNotInitialized();
}
}}
